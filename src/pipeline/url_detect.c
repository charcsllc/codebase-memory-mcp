/*
 * url_detect.c — Shared URL-argument / global-fetch detection for CALLS
 * resolution.
 *
 * Both pipeline paths must emit the same HTTP_CALLS evidence: URL-shaped
 * call arguments (via "arg_url") and direct global fetch() calls (via
 * "global_fetch"). These helpers previously lived as statics in
 * pass_parallel.c, which left the sequential path (repos under
 * MIN_FILES_FOR_PARALLEL, pass_calls.c) without either detection — the
 * same project produced different service evidence depending on which
 * pipeline mode kicked in.
 */
#include "pipeline/pipeline_internal.h"
#include "graph_buffer/graph_buffer.h"
#include "foundation/constants.h"
#include "foundation/str_util.h"
#include "service_patterns.h"
#include "cbm.h"

#include <stdio.h>
#include <string.h>

/* Reject regex metacharacters, spaces, double-slashes in URL candidates. */
static bool is_junk_url(const char *s) {
    for (int i = 0; s[i]; i++) {
        char ch = s[i];
        if (ch == '\\' || ch == '^' || ch == '$' || ch == '*' || ch == '+' || ch == '(' ||
            ch == ')' || ch == '[' || ch == ']' || ch == '|' || ch == ' ') {
            return true;
        }
        if (ch == '/' && i > 0 && s[i - SKIP_ONE] == '/') {
            return true;
        }
    }
    return false;
}

/* Normalize a template literal URL and reject junk patterns.
 * Returns true if norm contains a valid API path. */
static bool normalize_url_arg(const char *url, char *norm, int norm_sz) {
    int ni = 0;
    const char *p = url;
    if (*p == '`' || *p == '"' || *p == '\'') {
        p++;
    }
    if (*p != '/') {
        return false;
    }
    while (*p && ni < norm_sz - PAIR_LEN) {
        if (*p == '$' && *(p + SKIP_ONE) == '{') {
            /* Mid-segment interpolation ("/api/orders${qs}") is
             * querystring-style glue: treat it like '?' and stop, so the
             * path can match its server route instead of minting an
             * unmatchable ':qs' glued to the last segment. Segment-start
             * interpolation ("/api/users/${id}") stays a ':id' param. */
            if (ni == 0 || norm[ni - SKIP_ONE] != '/') {
                break;
            }
            norm[ni++] = ':';
            p += PAIR_LEN;
            while (*p && *p != '}' && ni < norm_sz - PAIR_LEN) {
                norm[ni++] = *p++;
            }
            if (*p == '}') {
                p++;
            }
        } else if (*p == '`' || *p == '"' || *p == '\'' || *p == '?') {
            break;
        } else {
            norm[ni++] = *p++;
        }
    }
    norm[ni] = '\0';
    enum { MIN_URL_LEN = 4 };
    if (ni < MIN_URL_LEN || !strchr(norm + SKIP_ONE, '/')) {
        return false;
    }
    return !is_junk_url(norm);
}

/* Detect API paths in call arguments and create HTTP_CALLS edges. */
void cbm_pipeline_detect_url_in_args(cbm_gbuf_t *gbuf, const cbm_gbuf_node_t *source,
                                     const CBMCall *call) {
    for (int ai = 0; ai < call->arg_count; ai++) {
        const CBMCallArg *ca = &call->args[ai];
        const char *url = ca->value ? ca->value : ca->expr;
        if (!url || (url[0] != '/' && url[0] != '`')) {
            continue;
        }
        char norm[CBM_SZ_256];
        if (!normalize_url_arg(url, norm, (int)sizeof(norm))) {
            continue;
        }
        char route_qn[CBM_ROUTE_QN_SIZE];
        char cpath[CBM_SZ_256];
        snprintf(route_qn, sizeof(route_qn), "__route__ANY__%s",
                 cbm_route_canon_path(norm, cpath, sizeof(cpath)));
        int64_t route_id = cbm_gbuf_upsert_node(gbuf, "Route", norm, route_qn, "", 0, 0,
                                                "{\"source\":\"arg_url\"}");
        char esc_c[CBM_SZ_256];
        char esc_n[CBM_SZ_256];
        cbm_json_escape(esc_c, sizeof(esc_c), call->callee_name);
        cbm_json_escape(esc_n, sizeof(esc_n), norm);
        char eprops[CBM_SZ_512];
        snprintf(eprops, sizeof(eprops),
                 "{\"callee\":\"%s\",\"url_path\":\"%s\",\"via\":\"arg_url\"}", esc_c, esc_n);
        cbm_gbuf_insert_edge(gbuf, source->id, route_id, "HTTP_CALLS", eprops);
        break;
    }
}

/* Exact global-fetch callee names. Deliberately NOT a service_patterns
 * substring table entry: "fetch" as a QN substring would false-positive on
 * fetchUser / prefetch / refetchQueries. */
bool cbm_pipeline_is_global_fetch(const char *callee) {
    return callee &&
           (strcmp(callee, "fetch") == 0 || strcmp(callee, "window.fetch") == 0 ||
            strcmp(callee, "globalThis.fetch") == 0);
}

/* Infer the HTTP method from a fetch init object literal in the args
 * ({ method: 'POST', ... }). Defaults to ANY (fetch's default is GET, but
 * ANY lets the route rendezvous match any server verb). */
static const char *fetch_init_method(const CBMCall *call) {
    static const char *const verbs[] = {"POST", "PUT", "PATCH", "DELETE", "OPTIONS", "HEAD",
                                        "GET"};
    for (int ai = 0; ai < call->arg_count; ai++) {
        const CBMCallArg *ca = &call->args[ai];
        if (!ca->expr) {
            continue;
        }
        const char *m = strstr(ca->expr, "method");
        if (!m) {
            continue;
        }
        for (size_t i = 0; i < sizeof(verbs) / sizeof(verbs[0]); i++) {
            if (strstr(m, verbs[i]) != NULL) {
                return verbs[i];
            }
        }
    }
    return "ANY";
}

/* Emit an HTTP_CALLS edge (plus its Route pseudo-node) for a direct call to
 * the global fetch(). The callee resolves to nothing (browser/Node builtin,
 * not in any library table), so the service classifiers never see it — the
 * unresolved path of each pipeline calls this instead. The URL argument
 * must pass the HTTP route-literal guard to avoid false positives. */
void cbm_pipeline_emit_global_fetch_edge(cbm_gbuf_t *gbuf, const cbm_gbuf_node_t *source,
                                         const CBMCall *call) {
    for (int ai = 0; ai < call->arg_count; ai++) {
        const CBMCallArg *ca = &call->args[ai];
        const char *url = ca->value ? ca->value : ca->expr;
        if (!url || (url[0] != '/' && url[0] != '`' && url[0] != '"' && url[0] != '\'')) {
            continue;
        }
        char norm[CBM_SZ_256];
        if (!normalize_url_arg(url, norm, (int)sizeof(norm))) {
            continue;
        }
        if (!cbm_service_pattern_is_http_route_literal(norm, call->callee_name)) {
            continue;
        }
        const char *method = fetch_init_method(call);
        char route_qn[CBM_ROUTE_QN_SIZE];
        char cpath[CBM_SZ_256];
        snprintf(route_qn, sizeof(route_qn), "__route__%s__%s", method,
                 cbm_route_canon_path(norm, cpath, sizeof(cpath)));
        int64_t route_id = cbm_gbuf_upsert_node(gbuf, "Route", norm, route_qn, "", 0, 0,
                                                "{\"source\":\"arg_url\"}");
        char esc_n[CBM_SZ_256];
        cbm_json_escape(esc_n, sizeof(esc_n), norm);
        char eprops[CBM_SZ_512];
        snprintf(eprops, sizeof(eprops),
                 "{\"callee\":\"fetch\",\"url_path\":\"%s\",\"method\":\"%s\","
                 "\"via\":\"global_fetch\"}",
                 esc_n, method);
        cbm_gbuf_insert_edge(gbuf, source->id, route_id, "HTTP_CALLS", eprops);
        return;
    }
}

#include "test_framework.h"
#include "graph_buffer/graph_buffer.h"
#include "pipeline/pipeline_internal.h"

#include <stdbool.h>
#include <stdint.h>

bool cbm_service_pattern_is_http_route_literal(const char *literal, const char *callee_name);

static int has_data_flow(cbm_gbuf_t *gb, int64_t source_id, int64_t target_id) {
    const cbm_gbuf_edge_t **edges = NULL;
    int count = 0;
    cbm_gbuf_find_edges_by_source_type(gb, source_id, "DATA_FLOWS", &edges, &count);
    for (int i = 0; i < count; i++) {
        if (edges[i]->target_id == target_id) {
            return 1;
        }
    }
    return 0;
}

TEST(infrascan_http_route_literal_guard_rejects_filesystem_paths) {
    ASSERT_FALSE(cbm_service_pattern_is_http_route_literal("/etc/crio/crio.conf", "requests.get"));
    ASSERT_FALSE(
        cbm_service_pattern_is_http_route_literal("/root/.aws/credentials", "requests.get"));
    ASSERT_FALSE(cbm_service_pattern_is_http_route_literal("/var/run/app.json", "requests.get"));
    ASSERT_FALSE(cbm_service_pattern_is_http_route_literal("/locations/", "str.split"));
    ASSERT_FALSE(cbm_service_pattern_is_http_route_literal("/api", "os.path.join"));
    ASSERT_FALSE(cbm_service_pattern_is_http_route_literal(NULL, "requests.get"));
    ASSERT_FALSE(cbm_service_pattern_is_http_route_literal("", "requests.get"));
    ASSERT_TRUE(cbm_service_pattern_is_http_route_literal("/api/orders", "requests.get"));
    ASSERT_TRUE(cbm_service_pattern_is_http_route_literal("https://orders.example/api/orders",
                                                          "requests.get"));
    PASS();
}

TEST(infrascan_route_nodes_skip_bad_http_url_paths) {
    cbm_gbuf_t *gb = cbm_gbuf_new("test", "/tmp/cbm_infrascan_route_guard");
    ASSERT_NOT_NULL(gb);
    int64_t caller =
        cbm_gbuf_upsert_node(gb, "Function", "client", "test.client", "client.py", 1, 3, "{}");
    int64_t fs_callee =
        cbm_gbuf_upsert_node(gb, "Function", "requests.get", "requests.get", "", 0, 0, "{}");
    int64_t split_callee =
        cbm_gbuf_upsert_node(gb, "Function", "str.split", "str.split", "", 0, 0, "{}");
    int64_t empty_callee =
        cbm_gbuf_upsert_node(gb, "Function", "requests.post", "requests.post", "", 0, 0, "{}");
    ASSERT_GT(caller, 0);
    ASSERT_GT(fs_callee, 0);
    ASSERT_GT(split_callee, 0);
    ASSERT_GT(empty_callee, 0);

    cbm_gbuf_insert_edge(gb, caller, fs_callee, "HTTP_CALLS",
                         "{\"callee\":\"requests.get\",\"url_path\":\"/etc/crio/crio.conf\","
                         "\"method\":\"GET\"}");
    cbm_gbuf_insert_edge(gb, caller, split_callee, "HTTP_CALLS",
                         "{\"callee\":\"str.split\",\"url_path\":\"/locations/\","
                         "\"method\":\"ANY\"}");
    cbm_gbuf_insert_edge(gb, caller, empty_callee, "HTTP_CALLS",
                         "{\"callee\":\"requests.get\",\"method\":\"GET\"}");

    cbm_pipeline_create_route_nodes(gb);

    ASSERT_NULL(cbm_gbuf_find_by_qn(gb, "__route__GET__/etc/crio/crio.conf"));
    ASSERT_NULL(cbm_gbuf_find_by_qn(gb, "__route__ANY__/locations/"));
    ASSERT_NULL(cbm_gbuf_find_by_qn(gb, "__route__GET__"));

    cbm_gbuf_free(gb);
    PASS();
}

TEST(infrascan_http_calls_join_matching_handler_route) {
    cbm_gbuf_t *gb = cbm_gbuf_new("test", "/tmp/cbm_infrascan_route_join");
    ASSERT_NOT_NULL(gb);

    int64_t route = cbm_gbuf_upsert_node(gb, "Route", "/api/orders", "__route__GET__/api/orders",
                                         "", 0, 0, "{\"method\":\"GET\"}");
    int64_t handler = cbm_gbuf_upsert_node(gb, "Function", "get_orders", "test.get_orders",
                                           "server.py", 1, 3, "{}");
    int64_t client =
        cbm_gbuf_upsert_node(gb, "Function", "client", "test.client", "client.py", 1, 3, "{}");
    int64_t bad_route =
        cbm_gbuf_upsert_node(gb, "Route", "/etc/crio/crio.conf",
                             "__route__GET__/etc/crio/crio.conf", "", 0, 0, "{\"method\":\"GET\"}");
    int64_t bad_handler = cbm_gbuf_upsert_node(gb, "Function", "bad_handler", "test.bad_handler",
                                               "server.py", 5, 7, "{}");
    int64_t bad_client = cbm_gbuf_upsert_node(gb, "Function", "bad_client", "test.bad_client",
                                              "client.py", 5, 7, "{}");
    ASSERT_GT(route, 0);
    ASSERT_GT(handler, 0);
    ASSERT_GT(client, 0);
    ASSERT_GT(bad_route, 0);
    ASSERT_GT(bad_handler, 0);
    ASSERT_GT(bad_client, 0);

    cbm_gbuf_insert_edge(gb, handler, route, "HANDLES", "{\"handler\":\"test.get_orders\"}");
    cbm_gbuf_insert_edge(gb, client, route, "HTTP_CALLS",
                         "{\"callee\":\"requests.get\",\"url_path\":\"/api/orders\","
                         "\"method\":\"GET\"}");
    cbm_gbuf_insert_edge(gb, bad_handler, bad_route, "HANDLES",
                         "{\"handler\":\"test.bad_handler\"}");
    cbm_gbuf_insert_edge(gb, bad_client, bad_route, "HTTP_CALLS",
                         "{\"callee\":\"requests.get\",\"url_path\":\"/etc/crio/crio.conf\","
                         "\"method\":\"GET\"}");

    cbm_pipeline_create_route_nodes(gb);

    ASSERT_TRUE(has_data_flow(gb, client, handler));
    ASSERT_FALSE(has_data_flow(gb, bad_client, bad_handler));

    cbm_gbuf_free(gb);
    PASS();
}

static int has_handles_edge(cbm_gbuf_t *gb, int64_t source_id, int64_t target_id) {
    const cbm_gbuf_edge_t **edges = NULL;
    int count = 0;
    cbm_gbuf_find_edges_by_source_type(gb, source_id, "HANDLES", &edges, &count);
    for (int i = 0; i < count; i++) {
        if (edges[i]->target_id == target_id) {
            return 1;
        }
    }
    return 0;
}

/* D6: Next.js App Router — route.ts files under the app tree exporting verb
 * handlers must produce Route nodes whose canon matches the server
 * framework's ([id] → {}) plus HANDLES edges from the handler Functions. */
TEST(infrascan_nextjs_app_router_route_nodes) {
    cbm_gbuf_t *gb = cbm_gbuf_new("test", "/tmp/cbm_nextjs_routes");
    ASSERT_NOT_NULL(gb);

    const char *rts = "frontend/src/app/api/x/[id]/route.ts";
    int64_t file = cbm_gbuf_upsert_node(gb, "File", "route.ts", "test.app.api.x.id.route", rts, 0,
                                        0, "{}");
    int64_t get_fn =
        cbm_gbuf_upsert_node(gb, "Function", "GET", "test.app.api.x.id.route.GET", rts, 3, 9, "{}");
    int64_t del_fn = cbm_gbuf_upsert_node(gb, "Function", "DELETE", "test.app.api.x.id.route.DELETE",
                                          rts, 11, 15, "{}");
    int64_t helper = cbm_gbuf_upsert_node(gb, "Function", "buildPayload",
                                          "test.app.api.x.id.route.buildPayload", rts, 17, 19, "{}");
    ASSERT_GT(file, 0);
    cbm_gbuf_insert_edge(gb, file, get_fn, "DEFINES", "{}");
    cbm_gbuf_insert_edge(gb, file, del_fn, "DEFINES", "{}");
    cbm_gbuf_insert_edge(gb, file, helper, "DEFINES", "{}");

    /* Route group "(shop)" segments vanish from the URL. */
    const char *grp = "frontend/src/app/(shop)/api/cart/route.ts";
    int64_t gfile =
        cbm_gbuf_upsert_node(gb, "File", "route.ts", "test.app.shop.api.cart.route", grp, 0, 0, "{}");
    int64_t gpost = cbm_gbuf_upsert_node(gb, "Function", "POST", "test.app.shop.api.cart.route.POST",
                                         grp, 1, 5, "{}");
    cbm_gbuf_insert_edge(gb, gfile, gpost, "DEFINES", "{}");

    /* _private folders are opted out of routing entirely. */
    const char *priv = "frontend/src/app/_lib/api/route.ts";
    int64_t pfile =
        cbm_gbuf_upsert_node(gb, "File", "route.ts", "test.app.lib.api.route", priv, 0, 0, "{}");
    int64_t pget = cbm_gbuf_upsert_node(gb, "Function", "GET", "test.app.lib.api.route.GET", priv,
                                        1, 3, "{}");
    cbm_gbuf_insert_edge(gb, pfile, pget, "DEFINES", "{}");

    cbm_pipeline_create_route_nodes(gb);

    const cbm_gbuf_node_t *route_get = cbm_gbuf_find_by_qn(gb, "__route__GET__/api/x/{}");
    const cbm_gbuf_node_t *route_del = cbm_gbuf_find_by_qn(gb, "__route__DELETE__/api/x/{}");
    const cbm_gbuf_node_t *route_cart = cbm_gbuf_find_by_qn(gb, "__route__POST__/api/cart");
    ASSERT_NOT_NULL(route_get);
    ASSERT_NOT_NULL(route_del);
    ASSERT_NOT_NULL(route_cart);
    ASSERT_TRUE(has_handles_edge(gb, get_fn, route_get->id));
    ASSERT_TRUE(has_handles_edge(gb, del_fn, route_del->id));
    ASSERT_TRUE(has_handles_edge(gb, gpost, route_cart->id));

    /* Non-verb exports and _private files create nothing. */
    ASSERT_NULL(cbm_gbuf_find_by_qn(gb, "__route__ANY__/api/x/{}"));
    ASSERT_NULL(cbm_gbuf_find_by_qn(gb, "__route__GET__/api"));
    ASSERT_NULL(cbm_gbuf_find_by_qn(gb, "__route__GET__/_lib/api"));
    ASSERT_NULL(cbm_gbuf_find_by_qn(gb, "__route__GET__/lib/api"));

    cbm_gbuf_free(gb);
    PASS();
}

static int has_edge_of_type(cbm_gbuf_t *gb, int64_t source_id, int64_t target_id,
                            const char *type) {
    const cbm_gbuf_edge_t **edges = NULL;
    int count = 0;
    cbm_gbuf_find_edges_by_source_type(gb, source_id, type, &edges, &count);
    for (int i = 0; i < count; i++) {
        if (edges[i]->target_id == target_id) {
            return 1;
        }
    }
    return 0;
}

/* D7: a client-side ANY pseudo-route must be mapped (INFRA_MAPS) onto the
 * server-side method route with the same canonical path, so DATA_FLOWS
 * reach the handler. A trailing ":ident" glued to the path (querystring
 * interpolation artifact) is trimmed for the match. */
TEST(infrascan_any_route_maps_to_method_route) {
    cbm_gbuf_t *gb = cbm_gbuf_new("test", "/tmp/cbm_any_method");
    ASSERT_NOT_NULL(gb);

    int64_t client =
        cbm_gbuf_upsert_node(gb, "Function", "loadY", "test.client.loadY", "client.ts", 1, 3, "{}");
    int64_t handler = cbm_gbuf_upsert_node(gb, "Function", "getY", "test.server.getY", "server.ts",
                                           1, 3, "{}");
    int64_t any1 = cbm_gbuf_upsert_node(gb, "Route", "/api/y", "__route__ANY__/api/y", "", 0, 0,
                                        "{\"method\":\"ANY\"}");
    int64_t get1 = cbm_gbuf_upsert_node(gb, "Route", "/api/y", "__route__GET__/api/y", "", 0, 0,
                                        "{\"method\":\"GET\"}");
    int64_t anyqs = cbm_gbuf_upsert_node(gb, "Route", "/api/z:qs", "__route__ANY__/api/z:qs", "", 0,
                                         0, "{\"method\":\"ANY\"}");
    int64_t getz = cbm_gbuf_upsert_node(gb, "Route", "/api/z", "__route__GET__/api/z", "", 0, 0,
                                        "{\"method\":\"GET\"}");
    ASSERT_GT(any1, 0);
    cbm_gbuf_insert_edge(gb, handler, get1, "HANDLES", "{\"handler\":\"test.server.getY\"}");
    cbm_gbuf_insert_edge(gb, handler, getz, "HANDLES", "{\"handler\":\"test.server.getY\"}");
    cbm_gbuf_insert_edge(gb, client, any1, "HTTP_CALLS",
                         "{\"callee\":\"fetch\",\"url_path\":\"/api/y\",\"via\":\"arg_url\"}");
    cbm_gbuf_insert_edge(gb, client, anyqs, "HTTP_CALLS",
                         "{\"callee\":\"fetch\",\"url_path\":\"/api/z:qs\",\"via\":\"arg_url\"}");

    cbm_pipeline_create_route_nodes(gb);

    ASSERT_TRUE(has_edge_of_type(gb, any1, get1, "INFRA_MAPS"));
    ASSERT_TRUE(has_edge_of_type(gb, anyqs, getz, "INFRA_MAPS"));
    /* The payoff: the client's data flow reaches the real handler. */
    ASSERT_TRUE(has_data_flow(gb, client, handler));

    cbm_gbuf_free(gb);
    PASS();
}

SUITE(infrascan) {
    RUN_TEST(infrascan_http_route_literal_guard_rejects_filesystem_paths);
    RUN_TEST(infrascan_route_nodes_skip_bad_http_url_paths);
    RUN_TEST(infrascan_http_calls_join_matching_handler_route);
    RUN_TEST(infrascan_nextjs_app_router_route_nodes);
    RUN_TEST(infrascan_any_route_maps_to_method_route);
}

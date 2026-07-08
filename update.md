# update.md — Mejoras de la rama `fix/graph-accuracy`

Registro de todas las mejoras de esta rama, verificadas contra el banco de pruebas
`ecommerce_pets` (monorepo TS: Fastify + Next.js 16) con un arnés de aceptación de
18 criterios (A1-A12 + guardarraíles G1-G7), un banco de fidelidad de resolución de
11 lenguajes, y la suite completa (5700+ tests, ASan+UBSan) en verde tras cada
cambio. Metodología: test rojo que reproduce la causa → fix mínimo → verde, con
diff de aristas fuertes (G1) para prohibir pérdidas de calidad.

## 1. Precisión del grafo (el grafo ya no miente)

| Commit | Mejora | Antes → Después |
|---|---|---|
| `8b747a9` | **Self-loops CALLS espurios eliminados.** `redis.get('k')`, `Map.get()` no resueltos emitían `a→a` con conf falsa; contaminaban el flag `recursive` y el fan-in de hotspots. | 37/487 self-loops → **0**; `recursive=true` solo con recursión real |
| `39d5240` | **El objeto de opciones de Fastify ya no es el handler.** `app.post(path, OPTS, handler)` elegía el primer arg; ahora gana el último function-like. | `AUTH_RATE_LIMIT-[HANDLES]->Route` (2 falsos) → 0; handler real enlazado |
| `5f0a101` | **Resolución por nombre pelado domada.** Llamadas con receptor (`redis.set`) y parámetros-callback ya no se inventan aristas cross-workspace vía `unique_name`; supresión de métodos genéricos JS espejando el guard Perl, con excepción para QNs de patrón de servicio (bullmq/sqs). | fan-in falso de `ProductForm.set`: 19 → **0**; fantasmas `createCheckout→CartProvider.add` etc. eliminados |
| `7dbbe9d` | **Cypher: alias `path`/`line`/`line_number`.** `RETURN f.path` devolvía `""`. | vacío → `frontend/src/lib/api/admin.ts` / `67` |
| `7de6a46` | **Cypher: variable repetida impuesta.** `MATCH (a)-[:CALLS]->(a)` devolvía TODAS las aristas. | ahora solo self-loops reales |
| `bc9e22d` | **trace_path sin duplicados.** BFS reportaba el mismo nodo en hop 1 y 2. | cada nodo una vez, en su hop mínimo |
| `a4ad6cd` | **Prisma etiquetado con verdad.** generator/datasource ya no son "Class"; los model son "Model". | 27 Class → 15 Class + 10 Model |

## 2. Cobertura (lo que el grafo no veía)

| Commit | Mejora | Antes → Después |
|---|---|---|
| `1c133bd` | **`await f<T>(...)` ya no pierde la llamada** (unwrap de await/paren/non-null en el callee). | `listAdminCategories` out_degree 0 → arista a `authedGet` presente |
| `cc5e0f0` | **Secretos excluidos del índice en todo modo** (`.env`, `.pem`, claves; `.env.example` permitido; re-inclusión vía `!` en `.cbmignore`; grep fallback blindado). | `backend/.env` indexado y buscable → **0 fugas** |
| `825ec87` | **Rutas Next.js App Router** (`app/**/route.ts`, `[param]`→`{}`, grupos `(g)` fuera, `@slots`/`_private` ignorados). | 0 → 18 rutas con HANDLES |
| `19f479a`+`901ff11`+`ccd1aed` (D7) | **fetch global + template URLs + rendezvous ANY→método** (`INFRA_MAPS` conecta pseudo-rutas de cliente con rutas de servidor; grados de search_graph cuentan aristas de servicio). | HTTP_CALLS 9 → 28; pseudo-rutas `:qs` conectadas; DATA_FLOWS 17 → 43 |
| `901ff11` | **Handlers inline anónimos con identidad** (`__handler_L<línea>` sintético + HANDLES; la atribución de llamadas del cuerpo no se mueve — estabilidad de G1). | HANDLES 18 → 57 (las 39 rutas Fastify con handler) |
| `19f479a` | **Paridad secuencial/paralelo** para detección de URLs y fetch (TU compartida `url_detect.c`). | repos <50 archivos: 0 → misma evidencia HTTP que el paralelo |

## 3. Fidelidad del mapa de imports (la materia prima de las aristas)

| Commit | Mejora | Antes → Después |
|---|---|---|
| `eb566ba` | **Basenames con punto estilo NestJS** (`./pedidos.repository` ya no pierde su último segmento; solo se recortan extensiones JS conocidas; retry sin segmento para extensiones desconocidas reales). | imports `x.service`/`x.repository` apuntaban al Folder equivocado → módulo exacto |
| `4d34ca6` | **Todos los bindings de un import sobreviven.** El dedup de aristas conservaba solo el primer `local_name` por módulo; ahora la arista lleva todos (coma-join) y los builders los separan. Maquinaria unificada en `pass_pkgmap.c` (antes 2 copias "keep in sync"). | ecommerce_pets: `import_map` 85 → **228**, `unique_name` 147 → **7**; `repo.*` 66/66 exactos |
| — | Única "pérdida" G1 de la rama, verificada contra el fuente: `buildApp→usuarios.service.register` era **falso positivo** (`app.register` de Fastify ≠ registro de usuarios); degradada de 0.75 a 0.38. | un error menos con cara de verdad |

## 4. Fidelidad multi-lenguaje (banco de 11 lenguajes)

Banco de medición: fixture idiomática por lenguaje (lib + distractor homónimo en otro
módulo + caller con import de miembro), clasificando la arista clave como
STRONG (import/receptor confirmado) / WEAK (correcta por puntuación) / WRONG (al
distractor) / MISSING.

**Resultado: de 12 STRONG · 8 WEAK · 2 WRONG → 22 STRONG · 3 WEAK · 0 WRONG.**

| Commit | Mejora | Lenguajes beneficiados |
|---|---|---|
| `48e5111` | **Cola cualificada confirma candidato único.** `Store.findWidgets` → `p.m.Store.findWidgets` es evidencia de receptor (0.9), no suerte de nombre único (0.75); receptor que no casa se queda en la vía débil. | C++, Java, C#, Perl, Ruby, Rust, PHP (×2 cada uno) |
| `c3d02f1` | **PHP: llamadas estáticas con receptor.** `Store::findWidgets()` extraía solo `findWidgets` (campo `scope` ignorado) — el distractor GANABA por distancia. | PHP: WRONG → STRONG |
| `b9d8867` | **Scala: package + imports extraídos.** `package_clause` no se capturaba y el parser genérico tomaba solo el primer identificador (`com`) del path plano del grammar. Parser propio con selectores `{A, B}` y wildcard. | Scala: WEAK 0.41 → import_map 0.95 |
| `1c8102f` | **Mapa de imports consciente de namespaces.** Los valores del mapa eran QNs `__file__` (imposibles de casar) y el builder result-based no consultaba el namespace_map — PHP/Java/C#/Scala no-relativos quedaban fuera. Valores = QN de módulo (sufijo `.__file__` recortado, ownership del mapa) y `pass_calls` construye el namespace_map una vez por pasada. | PHP `use function`: WRONG → import_map 0.95; Scala; cualquier lenguaje de namespaces |

WEAK restantes, correctos y documentados: C (headers sin semántica de import → el
target correcto gana por puntuación) y Rust `use mod::fn` de función suelta
(unique_name 0.75 al target correcto).

## 4b. Dieta de tokens en las respuestas MCP

Las respuestas de las tools las consume un LLM: cada byte redundante quema la
ventana de contexto del agente en cada llamada. Cortes sin pérdida (round-trip
garantizado), medidos sobre 9 llamadas típicas en ecommerce_pets:
**21.7 KB → 18.1 KB (−17%; trace_path −23%, get_architecture −25%,
index_status −55%, list_projects −53%)**.

| Corte | Detalle |
|---|---|
| Prefijo de proyecto elidido en QNs | La petición ya está acotada a un proyecto; el slug repetía ~9 tokens por fila. `get_code_snippet` resuelve formas sin prefijo (tier de sufijo), `search_graph` reintenta `qn_pattern` con prefijo, `query_graph` queda verbatim (Cypher crudo compara valores almacenados). |
| Properties sin defaults | `loop_count:0`, `is_test:false`, strings vacíos y el `fingerprint` interno (hash de similitud inaccionable) se omiten — eran el grueso del peso por fila de search_graph. |
| Campos estructuralmente redundantes | `label` por fila cuando la query ya filtró por label; `file_path` vacíos; path RELATIVO en snippet (el absoluto repetía la raíz del repo); `expected_*` solo si discrepa; git block de list_projects a campos de identidad; adr_hint de una línea. |

## 5. Robustez operacional

| Commit | Mejora |
|---|---|
| `861f612` | **`install --force` con el servidor corriendo.** `fopen("wb")` sobre el binario en ejecución da ETXTBSY; ahora unlink-then-copy (Windows: rename-aside), espejando `cbm_replace_binary`. |
| `95013b5` | **Instancias identificadas por ejecutable, no por nombre.** `pgrep -x codebase-memory-mcp` no puede casar en Linux (comm truncado a 15 chars) — los servidores viejos sobrevivían cada upgrade. Ahora `/proc/<pid>/exe` (con manejo de `" (deleted)"`) en Linux, libproc en macOS; SIGTERM → gracia acotada → SIGKILL. Verificado en vivo: "Stopped 2 running MCP server instance(s)". |
| `f17a8cb` | **Use-after-free latente cazado y eliminado.** Quince tests de slab destruían el allocator del thread sin destruir antes los TSParsers cacheados (que asignan sus internals DESDE el slab, contrato documentado en slab_alloc.h) — el siguiente parse en ese thread leía estado del lexer liberado (ASan: `ts_lexer_goto`, detonando dos suites después). Producción ya respetaba el orden; los tests no. Destapado precisamente por los fixtures nuevos del banco de lenguajes. |
| `a9ac0e6` | **test-runner con filtro de suites** (`build/c/test-runner parallel cli` = segundos vs ~10 min de ASan) y fflush de resultados para que un abort de LeakSanitizer no se trague el nombre del test que falló. |

## 6. Estado de verificación

- **Suite completa**: 5700+ tests en verde (ASan+UBSan). Único fallo permanente en
  esta máquina: `test_incremental.c:302`, umbral de RSS (2 GB) calibrado para el
  sparse checkout de CI — ambiental, no funcional (~2.58 GB con checkout completo).
- **Arnés ecommerce_pets**: 18/18 PASS tras cada cambio; diff G1 de aristas fuertes
  con cero pérdidas no justificadas en toda la rama.
- **Lint**: `lint-no-suppress` + whitelist NOLINT en verde (clang-tidy/cppcheck no
  disponibles en esta máquina; pendientes de CI).

## 7. Deuda conocida (anotada, no olvidada)

- Arrows en props JSX sin nodo propio (granularidad de atribución, no pérdida de
  aristas — re-atribuir movería sources de CALLS existentes; requiere decisión de
  semántica).
- Auditoría del pipeline **incremental** con la nueva maquinaria de imports
  (posible formato viejo de `local_name` en reindex parcial).
- Gitignore anidado multinivel; fallback PowerShell de search_code sin verificar
  en Windows; `pgrep` legacy en POSIX no-Linux/macOS.
- Pico de RSS del pipeline (~2.5 GB en repos grandes): la palanca arquitectónica
  pendiente (liberación eager de `CBMFileResult`).
- C/C++ `#include` sin mapa de imports real (WEAK correcto); Rust `use` de función
  suelta sin entrada de mapa.

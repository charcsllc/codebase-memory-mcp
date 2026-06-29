# codebase-memory-mcp

[![GitHub Release](https://img.shields.io/github/v/release/DeusData/codebase-memory-mcp?style=flat&color=blue)](https://github.com/DeusData/codebase-memory-mcp/releases/latest)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![CI](https://img.shields.io/github/actions/workflow/status/DeusData/codebase-memory-mcp/dry-run.yml?label=CI)](https://github.com/DeusData/codebase-memory-mcp/actions/workflows/dry-run.yml)
[![Tests](https://img.shields.io/badge/tests-5604_passing-brightgreen)](https://github.com/DeusData/codebase-memory-mcp)
[![Languages](https://img.shields.io/badge/languages-158-orange)](https://github.com/DeusData/codebase-memory-mcp)
[![Hybrid LSP](https://img.shields.io/badge/Hybrid_LSP-9_languages-blue)](#hybrid-lsp)
[![Agents](https://img.shields.io/badge/agents-11-purple)](https://github.com/DeusData/codebase-memory-mcp)
[![Pure C](https://img.shields.io/badge/pure_C-zero_dependencies-blue)](https://github.com/DeusData/codebase-memory-mcp)
[![Platform](https://img.shields.io/badge/macOS_%7C_Linux_%7C_Windows-supported-lightgrey)](https://github.com/DeusData/codebase-memory-mcp/releases/latest)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/DeusData/codebase-memory-mcp/badge)](https://scorecard.dev/viewer/?uri=github.com/DeusData/codebase-memory-mcp)
[![SLSA 3](https://slsa.dev/images/gh-badge-level3.svg)](https://slsa.dev)
[![VirusTotal](https://img.shields.io/badge/VirusTotal-scanned_every_release-brightgreen?logo=virustotal)](https://github.com/DeusData/codebase-memory-mcp/releases/latest)
[![arXiv](https://img.shields.io/badge/arXiv-2603.27277-b31b1b?logo=arxiv)](https://arxiv.org/abs/2603.27277)

**El motor de inteligencia de código más rápido y eficiente para agentes de IA de programación.** Indexa por completo un repositorio promedio en milisegundos, y el kernel de Linux (28M de líneas, 75K archivos) en 3 minutos. Responde consultas estructurales en menos de 1 ms. Se distribuye como un único binario estático para macOS, Linux y Windows — descárgalo, ejecuta `install` y listo.

Análisis de alta calidad mediante AST de [tree-sitter](https://tree-sitter.github.io/tree-sitter/) en los 158 lenguajes, potenciado con [**resolución semántica de tipos Hybrid LSP**](#hybrid-lsp) para Python, TypeScript / JavaScript / JSX / TSX, PHP, C#, Go, C, C++, Java, Kotlin y Rust — produciendo un grafo de conocimiento persistente de funciones, clases, cadenas de llamadas, rutas HTTP y enlaces entre servicios. 14 herramientas MCP. Cero dependencias. Compatible y listo para usar con 11 agentes de programación.

> **Investigación** — El diseño y los benchmarks de este proyecto se describen en el preprint [*Codebase-Memory: Tree-Sitter-Based Knowledge Graphs for LLM Code Exploration via MCP*](https://arxiv.org/abs/2603.27277) (arXiv:2603.27277). Evaluado en 31 repositorios reales: 83% de calidad de respuesta, 10× menos tokens y 2,1× menos llamadas a herramientas frente a la exploración archivo por archivo.

> **Seguridad y confianza** — Esta herramienta lee tu base de código y escribe en los archivos de configuración de tu agente. Eso es exactamente para lo que está diseñada. Si prefieres auditarla antes de ejecutarla, el [código fuente completo está aquí](https://github.com/DeusData/codebase-memory-mcp) — cada binario publicado está firmado, con suma de verificación y escaneado por más de 70 motores antivirus. Todo el procesamiento ocurre 100% en local; tu código nunca sale de tu máquina. **Sin telemetría y sin "phone-home": el servidor MCP no realiza ninguna llamada de red automática al arrancar, durante la indexación ni durante el uso** (consulta [Privacidad y telemetría](#privacidad-y-telemetría)). ¿Encontraste un problema de seguridad? Queremos saberlo — consulta [SECURITY.md](SECURITY.md). La seguridad es nuestra prioridad #1.

<p align="center">
  <img src="docs/graph-ui-screenshot.png" alt="Graph visualization UI showing the codebase-memory-mcp knowledge graph" width="800">
  <br>
  <em>Visualización de grafo 3D integrada (variante UI) — explora tu grafo de conocimiento en localhost:9749</em>
</p>

## Por qué codebase-memory-mcp

- **Velocidad de indexación extrema** — el kernel de Linux (28M de líneas, 75K archivos) en 3 minutos. Pipeline que prioriza la RAM: compresión LZ4, SQLite en memoria y coincidencia de patrones Aho-Corasick fusionada. La memoria se libera tras la indexación.
- **Listo para usar** — un único binario estático para macOS (arm64/amd64), Linux (arm64/amd64) y Windows (amd64). Sin Docker, sin dependencias de runtime, sin claves de API. Descargar → `install` → reiniciar el agente → listo.
- **158 lenguajes** — gramáticas tree-sitter incorporadas y compiladas dentro del binario. Nada que instalar, nada que se rompa.
- **120× menos tokens** — 5 consultas estructurales: ~3.400 tokens frente a ~412.000 con búsqueda archivo por archivo. Una consulta al grafo reemplaza decenas de ciclos de grep/lectura.
- **11 agentes, un solo comando** — `install` detecta automáticamente Claude Code, Codex CLI, Gemini CLI, Zed, OpenCode, Antigravity, Aider, KiloCode, VS Code, OpenClaw y Kiro — y configura las entradas MCP, los archivos de instrucciones y los hooks de pre-herramienta para cada uno.
- **Visualización de grafo integrada** — interfaz 3D interactiva en `localhost:9749` (variante de binario con UI, opcional).
- **Indexación de infraestructura como código** — Dockerfiles, manifiestos de Kubernetes y overlays de Kustomize indexados como nodos del grafo con referencias cruzadas. Nodos `Resource` para los tipos de K8s, nodos `Module` para los overlays de Kustomize con aristas `IMPORTS` hacia los recursos referenciados.
- **14 herramientas MCP** — búsqueda, trazado, arquitectura, análisis de impacto, consultas Cypher, detección de código muerto, enlace HTTP entre servicios, gestión de ADR y más.

## Inicio rápido

**Instalación en una línea** (macOS / Linux):
```bash
curl -fsSL https://raw.githubusercontent.com/DeusData/codebase-memory-mcp/main/install.sh | bash
```

Con interfaz de visualización de grafo:
```bash
curl -fsSL https://raw.githubusercontent.com/DeusData/codebase-memory-mcp/main/install.sh | bash -s -- --ui
```

**Windows** (PowerShell):
```powershell
# 1. Descarga el instalador
Invoke-WebRequest -Uri https://raw.githubusercontent.com/DeusData/codebase-memory-mcp/main/install.ps1 -OutFile install.ps1

# 2. (Opcional pero recomendado) Inspecciona el script
notepad install.ps1

# 3. Ejecútalo
.\install.ps1

```

Opciones: `--ui` (visualización de grafo), `--skip-config` (solo binario, sin configurar agentes), `--dir=<path>` (ubicación personalizada).

Reinicia tu agente de programación. Di **"Indexa este proyecto"** — listo.

> **Compilación desde el código fuente (sin telemetría)** — Los scripts de instalación de arriba **descargan** el binario oficial publicado desde GitHub. Si prefieres ejecutar un binario que compilas tú mismo a partir de este código fuente (por ejemplo, esta copia con la comprobación automática de actualizaciones eliminada), consulta [Compilar desde el código fuente](#compilar-desde-el-código-fuente) y luego instala con el subcomando `install` del binario, que no accede a la red. Consulta [Privacidad y telemetría](#privacidad-y-telemetría).

<details>
<summary>Instalación manual</summary>

1. **Descarga** el archivo comprimido para tu plataforma desde la [última versión publicada](https://github.com/DeusData/codebase-memory-mcp/releases/latest):
   - `codebase-memory-mcp-<os>-<arch>.tar.gz` (macOS/Linux) o `.zip` (Windows) — estándar
   - `codebase-memory-mcp-ui-<os>-<arch>.tar.gz` / `.zip` — con visualización de grafo

2. **Extrae e instala** (cada archivo incluye `install.sh` o `install.ps1`):

   macOS / Linux:
   ```bash
   tar xzf codebase-memory-mcp-*.tar.gz
   ./install.sh
   ```

   Windows (PowerShell):
   ```powershell
   Expand-Archive codebase-memory-mcp-windows-amd64.zip -DestinationPath .
   .\install.ps1
   ```

3. **Reinicia** tu agente de programación.

El comando `install` elimina automáticamente los atributos de cuarentena de macOS y firma el binario de forma ad-hoc — sin necesidad de `xattr`/`codesign` manuales.
</details>

El comando `install` detecta automáticamente todos los agentes de programación instalados y configura las entradas del servidor MCP, los archivos de instrucciones, las skills y los hooks de pre-herramienta para cada uno.

### Interfaz de visualización de grafo

Si descargaste la variante `ui`:

```bash
codebase-memory-mcp --ui=true --port=9749
```

Abre `http://localhost:9749` en tu navegador. La interfaz se ejecuta como un hilo en segundo plano junto al servidor MCP — está disponible siempre que tu agente esté conectado.

### Indexación automática

Activa la indexación automática al iniciar la sesión MCP:

```bash
codebase-memory-mcp config set auto_index true
```

Cuando está activada, los proyectos nuevos se indexan automáticamente en la primera conexión. Los proyectos ya indexados se registran en el observador en segundo plano para la detección continua de cambios basada en git. Límite de archivos configurable: `config set auto_index_limit 50000`.

### Mantenerse actualizado

```bash
codebase-memory-mcp update
```

`update` es el único comando que accede a la red, y se ejecuta **únicamente cuando lo invocas de forma explícita**. El servidor MCP no comprueba actualizaciones de forma automática: no realiza ninguna llamada de red al arrancar ni durante la indexación. (La comprobación automática de actualizaciones que existía en versiones anteriores se eliminó — consulta [Privacidad y telemetría](#privacidad-y-telemetría).)

### Desinstalación

```bash
codebase-memory-mcp uninstall
```

Elimina todas las configuraciones de agentes, skills, hooks e instrucciones. No elimina el binario ni las bases de datos SQLite.

## Características

### Grafo y análisis
- **Resumen de arquitectura**: `get_architecture` devuelve lenguajes, paquetes, puntos de entrada, rutas, hotspots, fronteras, capas y clústeres en una sola llamada
- **Architecture Decision Records**: `manage_adr` persiste las decisiones de arquitectura entre sesiones
- **Detección de comunidades Louvain**: descubre módulos funcionales agrupando las aristas de llamadas
- **Mapeo de impacto del diff de git**: `detect_changes` mapea los cambios sin confirmar a los símbolos afectados con clasificación de riesgo
- **Grafo de llamadas**: resuelve las llamadas a funciones entre archivos y paquetes (consciente de imports, con inferencia de tipos)
- **Detección de código muerto**: encuentra funciones sin ningún llamador, excluyendo los puntos de entrada
- **Consultas tipo Cypher**: `MATCH (f:Function)-[:CALLS]->(g) WHERE f.name = 'main' RETURN g.name`

### Búsqueda
- **Búsqueda semántica** (`semantic_query`): búsqueda vectorial en todo el grafo, impulsada por embeddings `nomic-embed-code` de Nomic incorporados (40K tokens, 768d int8) compilados dentro del binario — sin clave de API, sin Ollama, sin Docker. Puntuación combinada de 11 señales (TF-IDF, RRI, firmas de API/Tipo/Decorador, perfiles de AST, flujo de datos, Halstead-lite, MinHash, proximidad de módulos, difusión en el grafo).
- **Búsqueda de texto completo BM25** vía SQLite FTS5 con el tokenizador `cbm_camel_split` (consciente de camelCase / snake_case)
- **Búsqueda estructural** (`search_graph`): patrones regex de nombre, filtros por etiqueta, grado mínimo/máximo, ámbito por archivo
- **Búsqueda de código** (`search_code`): grep aumentado por el grafo, solo sobre los archivos indexados

### Enlace entre servicios
- **HTTP**: coincidencia entre rutas y sitios de llamada con puntuación de confianza
- **gRPC, GraphQL, tRPC**: detección de servicios con extracción de rutas (Route) desde protobuf
- **Detección de canales** (`EMITS` / `LISTENS_ON`) para Socket.IO, EventEmitter y patrones genéricos de pub-sub en 8 lenguajes con resolución de constantes

### Inteligencia entre repositorios
- **Aristas `CROSS_*`** que enlazan nodos a través de múltiples repos indexados en el mismo almacén
- **Disposición 3D multi-galaxia** para visualizar la arquitectura entre repositorios
- **Resumen de arquitectura entre repos** que combina servicios, rutas y dependencias de toda la flota indexada

### Tipos de arista (selección)
- `CALLS`, `IMPORTS`, `DEFINES`, `IMPLEMENTS`, `INHERITS`
- `HTTP_CALLS`, `ASYNC_CALLS` (entre servicios)
- `EMITS`, `LISTENS_ON` (canales)
- `DATA_FLOWS` con mapeo de argumento a parámetro + cadenas de acceso a campos
- `SIMILAR_TO` (detección de clones cercanos con MinHash + LSH, puntuada con Jaccard)
- `SEMANTICALLY_RELATED` (discordancia de vocabulario, mismo lenguaje, puntuación ≥ 0,80)

### Pipeline de indexación
- **158 gramáticas tree-sitter incorporadas** y compiladas dentro del binario
- **Resolución genérica de paquetes / módulos** — especificadores simples como `@myorg/pkg`, `github.com/foo/bar`, `use my_crate::foo` resueltos mediante el escaneo de manifiestos (`package.json`, `go.mod`, `Cargo.toml`, `pyproject.toml`, `composer.json`, `pubspec.yaml`, `pom.xml`, `build.gradle`, `mix.exs`, `*.gemspec`)
- **Indexación de infraestructura como código** — Dockerfiles, manifiestos de Kubernetes y overlays de Kustomize como nodos del grafo
- **[Resolución semántica de tipos Hybrid LSP](#hybrid-lsp)** para Python, TypeScript / JavaScript / JSX / TSX, PHP, C#, Go, C, C++, Java, Kotlin y Rust — una implementación ligera en C de algoritmos de resolución de tipos de lenguajes, estructuralmente inspirada en y compatible con los principales servidores de lenguaje, incluidos tsserver / typescript-go, pyright, gopls, Roslyn, Eclipse JDT y rust-analyzer (vinculación de parámetros, inferencia de tipo de retorno, sustitución de genéricos, despacho de componentes JSX, inferencia JSDoc para archivos JS planos, resolución de namespaces + traits + late-static-binding en PHP, namespaces con ámbito de archivo + records + sintaxis de método LINQ en C#, resolución de jerarquía de clases + sobrecargas + lambdas en Java, resolución de funciones de extensión + funciones de ámbito en Kotlin, resolución de métodos de trait + UFCS en Rust)
- **Pipeline que prioriza la RAM**: compresión LZ4, SQLite en memoria, un único volcado al final. La memoria se libera después.

### Distribución y operación
- **Binario estático único, cero infraestructura**: respaldado por SQLite, persiste en `~/.cache/codebase-memory-mcp/`
- **Auto-sincronización**: el observador en segundo plano detecta cambios de archivos y re-indexa automáticamente
- **Nodos de ruta**: los endpoints REST son entidades de primera clase en el grafo
- **Modo CLI**: `codebase-memory-mcp cli search_graph '{"name_pattern": ".*Handler.*"}'`
- **Disponible en**: npm, PyPI, Homebrew, Scoop, Winget, Chocolatey, AUR, `go install`

## Artefacto de grafo compartido por el equipo

Confirma un único archivo comprimido en tu repo y tus compañeros se ahorran la re-indexación.

`.codebase-memory/graph.db.zst` es una instantánea del grafo de conocimiento comprimida con zstd que vive junto a tu código fuente. Cuando indexas, el artefacto se escribe o se actualiza; cuando un compañero clona el repo y ejecuta `codebase-memory-mcp` por primera vez, el artefacto se descomprime y la indexación incremental rellena su diff local.

- **Formato**: base de datos SQLite, índices eliminados, compactada con `VACUUM INTO` y luego comprimida con zstd 1.5.7 (ratio típico de 8–13:1)
- **Dos niveles**:
  - **Best** (`zstd -9` + eliminación de índices + `VACUUM INTO`) — escrito en un `index_repository` explícito
  - **Fast** (`zstd -3`) — escrito por el observador para actualizaciones incrementales de baja latencia
- **Bootstrap**: cuando no existe una base de datos local pero el artefacto está presente, `index_repository` importa primero el artefacto y luego ejecuta la indexación incremental — evitando el coste de la re-indexación completa
- **Sin dolores de fusión**: una línea `merge=ours` en `.gitattributes` se crea automáticamente en la primera exportación, de modo que las ediciones concurrentes no producen conflictos en el artefacto binario
- **Opcional**: nunca se confirma a menos que tú quieras. Añade `.codebase-memory/` a tu `.gitignore` si prefieres que todos re-indexen desde cero.

El resultado es similar en espíritu al directorio `graphify-out/` de graphify, pero como un único archivo comprimido con exportación explícita de dos niveles, importación con verificación de integridad y cero fricción de fusión.

## Cómo funciona

codebase-memory-mcp es un **backend de análisis estructural** — construye y consulta el grafo de conocimiento. **No** incluye un LLM. En su lugar, depende de tu cliente MCP (Claude Code, o cualquier agente compatible con MCP) para que sea la capa de inteligencia.

```
You: "what calls ProcessOrder?"

Agent calls: trace_path(function_name="ProcessOrder", direction="inbound")

codebase-memory-mcp: executes graph query, returns structured results

Agent: presents the call chain in plain English
```

**¿Por qué no hay un LLM integrado?** Otras herramientas de grafo de código incrustan un LLM para traducir lenguaje natural → consulta de grafo. Eso implica claves de API adicionales, coste adicional y otro modelo que configurar. Con MCP, el agente con el que ya estás hablando *es* el traductor de consultas.

## Rendimiento

Medido en un Apple M3 Pro:

| Operación | Tiempo | Notas |
|-----------|------|-------|
| **Indexación completa del kernel de Linux** | **3 min** | 28M de líneas, 75K archivos → 4,81M nodos, 7,72M aristas |
| Indexación rápida del kernel de Linux | 1m 12s | 1,88M nodos |
| Indexación completa de Django | ~6s | 49K nodos, 196K aristas |
| Consulta Cypher | <1ms | Recorrido de relaciones |
| Búsqueda por nombre (regex) | <10ms | Pre-filtrado SQL LIKE |
| Detección de código muerto | ~150ms | Escaneo completo del grafo con filtrado por grado |
| Trazar ruta de llamadas (profundidad=5) | <10ms | Recorrido BFS |

**Pipeline que prioriza la RAM**: toda la indexación se ejecuta en memoria (lectura comprimida con LZ4 HC, SQLite en memoria, un único volcado al final). La memoria se devuelve al sistema operativo una vez completada la indexación.

**Eficiencia de tokens**: cinco consultas estructurales consumieron ~3.400 tokens con codebase-memory-mcp frente a ~412.000 tokens con la exploración por grep archivo por archivo — una **reducción del 99,2%**.

## Instalación

### Binarios precompilados

| Plataforma | Estándar | Con UI de grafo |
|----------|----------|---------------|
| macOS (Apple Silicon) | `codebase-memory-mcp-darwin-arm64.tar.gz` | `codebase-memory-mcp-ui-darwin-arm64.tar.gz` |
| macOS (Intel) | `codebase-memory-mcp-darwin-amd64.tar.gz` | `codebase-memory-mcp-ui-darwin-amd64.tar.gz` |
| Linux (x86_64) | `codebase-memory-mcp-linux-amd64.tar.gz` | `codebase-memory-mcp-ui-linux-amd64.tar.gz` |
| Linux (ARM64) | `codebase-memory-mcp-linux-arm64.tar.gz` | `codebase-memory-mcp-ui-linux-arm64.tar.gz` |
| Windows (x86_64) | `codebase-memory-mcp-windows-amd64.zip` | `codebase-memory-mcp-ui-windows-amd64.zip` |

Cada versión publicada incluye `checksums.txt` con hashes SHA-256. Todos los binarios están enlazados estáticamente — sin dependencias de bibliotecas compartidas.

> **Nota para Windows**: SmartScreen puede mostrar una advertencia para software sin firmar. Haz clic en **"Más información"** → **"Ejecutar de todas formas"**. Verifica la integridad con `checksums.txt`.

### Scripts de configuración

<details>
<summary>Descarga + instalación automatizadas</summary>

**macOS / Linux:**

```bash
curl -fsSL https://raw.githubusercontent.com/DeusData/codebase-memory-mcp/main/scripts/setup.sh | bash
```

**Windows (PowerShell):**

```powershell
irm https://raw.githubusercontent.com/DeusData/codebase-memory-mcp/main/scripts/setup-windows.ps1 | iex
```

</details>

### AUR (Arch Linux)

```bash
yay -S codebase-memory-mcp-bin
```

```bash
paru -S codebase-memory-mcp-bin
```

El paquete `codebase-memory-mcp-bin` está disponible en: https://aur.archlinux.org/packages/codebase-memory-mcp-bin

### Instalación vía Claude Code

```
You: "Install this MCP server: https://github.com/DeusData/codebase-memory-mcp"
```

### Compilar desde el código fuente

<details>
<summary>Requisitos previos: compilador de C + zlib</summary>

| Requisito | Comprobar | Instalar |
|-------------|-------|---------|
| **Compilador de C** (gcc o clang) | `gcc --version` o `clang --version` | macOS: `xcode-select --install`, Linux: `apt install build-essential` |
| **Compilador de C++** | `g++ --version` o `clang++ --version` | Igual que arriba |
| **zlib** | — | macOS: incluido, Linux: `apt install zlib1g-dev` |
| **Git** | `git --version` | Preinstalado en la mayoría de sistemas |

</details>

```bash
git clone https://github.com/DeusData/codebase-memory-mcp.git
cd codebase-memory-mcp
scripts/build.sh                    # binario estándar
scripts/build.sh --with-ui          # con visualización de grafo
# Binario en: build/c/codebase-memory-mcp
```

> En Windows con un toolchain MinGW antiguo puede que necesites compilar con `mingw32-make` y banderas de compatibilidad de cabeceras. El binario resultante, compilado a partir de este código fuente, no contiene la comprobación automática de actualizaciones.

### Configuración MCP manual

<details>
<summary>Si prefieres no usar el comando install</summary>

Añade a `~/.claude/.mcp.json` (global) o al `.mcp.json` del proyecto:

```json
{
  "mcpServers": {
    "codebase-memory-mcp": {
      "command": "/path/to/codebase-memory-mcp",
      "args": []
    }
  }
}
```

Reinicia tu agente. Verifica con `/mcp` — deberías ver `codebase-memory-mcp` con 14 herramientas.

</details>

## Soporte multi-agente

`install` detecta automáticamente y configura todos los agentes instalados:

| Agente | Config MCP | Instrucciones | Hooks |
|-------|-----------|-------------|-------|
| Claude Code | `.claude/.mcp.json` | 4 Skills | PreToolUse (aumento del grafo en Grep/Glob, no bloqueante) |
| Codex CLI | `.codex/config.toml` | `.codex/AGENTS.md` | Recordatorio en SessionStart |
| Gemini CLI | `.gemini/settings.json` | `.gemini/GEMINI.md` | BeforeTool (recordatorio de grep) + recordatorio en SessionStart |
| Zed | `settings.json` (JSONC) | — | — |
| OpenCode | `opencode.json` | `AGENTS.md` | — |
| Antigravity | `.gemini/config/mcp_config.json` (compartido) | `antigravity-cli/AGENTS.md` | Recordatorio en SessionStart |
| Aider | — | `CONVENTIONS.md` | — |
| KiloCode | `mcp_settings.json` | `~/.kilocode/rules/` | — |
| VS Code | `Code/User/mcp.json` | — | — |
| OpenClaw | `openclaw.json` | — | — |
| Kiro | `.kiro/settings/mcp.json` | — | — |

**Los hooks son estructuralmente no bloqueantes** (código de salida 0 en todas las rutas de fallo).
Para Claude Code, el hook `PreToolUse` intercepta `Grep`/`Glob` (nunca `Read` —
bloquear `Read` rompería la invariante de leer-antes-de-editar) y, cuando el
token de búsqueda coincide con símbolos indexados, los inyecta como
`additionalContext` vía `search_graph`, de modo que el agente obtiene contexto
estructurado junto a sus resultados de búsqueda normales. Para Codex, Gemini CLI
y Antigravity, un hook `SessionStart` inyecta como contexto de sesión un
recordatorio de una línea para descubrir código (Gemini CLI también conserva su
recordatorio `BeforeTool`).
El archivo shim de Claude que se instala se llama `cbm-code-discovery-gate` por
compatibilidad con instalaciones existentes; a pesar del nombre heredado, nunca
actúa como puerta ni bloquea.

## Modo CLI

Cada herramienta MCP se puede invocar desde la línea de comandos:

```bash
codebase-memory-mcp cli index_repository '{"repo_path": "/path/to/repo"}'
codebase-memory-mcp cli search_graph '{"name_pattern": ".*Handler.*", "label": "Function"}'
codebase-memory-mcp cli trace_path '{"function_name": "Search", "direction": "both"}'
codebase-memory-mcp cli query_graph '{"query": "MATCH (f:Function) RETURN f.name LIMIT 5"}'
codebase-memory-mcp cli list_projects
codebase-memory-mcp cli --raw search_graph '{"label": "Function"}' | jq '.results[].name'
```

## Herramientas MCP

### Indexación

| Herramienta | Descripción |
|------|-------------|
| `index_repository` | Indexa un repositorio en el grafo. La auto-sincronización lo mantiene fresco a partir de entonces. |
| `list_projects` | Lista todos los proyectos indexados con sus conteos de nodos/aristas. |
| `delete_project` | Elimina un proyecto y todos sus datos del grafo. |
| `index_status` | Comprueba el estado de indexación de un proyecto. |

### Consulta

| Herramienta | Descripción |
|------|-------------|
| `search_graph` | Búsqueda estructurada por etiqueta, patrón de nombre, patrón de archivo, filtros de grado. Paginación vía limit/offset. |
| `trace_path` | Recorrido BFS — quién llama a una función y a qué llama ella (alias: `trace_call_path`). Profundidad 1-5. |
| `detect_changes` | Mapea el diff de git a los símbolos afectados + radio de impacto con clasificación de riesgo. |
| `query_graph` | Ejecuta consultas de grafo tipo Cypher (solo lectura). |
| `get_graph_schema` | Conteos de nodos/aristas, patrones de relación, definiciones de propiedades por etiqueta. Ejecútala primero. |
| `get_code_snippet` | Lee el código fuente de una función por su nombre cualificado. |
| `get_architecture` | Resumen de la base de código: lenguajes, paquetes, rutas, hotspots, clústeres, ADR. |
| `search_code` | Búsqueda de texto tipo grep dentro de los archivos del proyecto indexado. |
| `manage_adr` | CRUD para Architecture Decision Records. |
| `ingest_traces` | Ingiere trazas en tiempo de ejecución para validar las aristas HTTP_CALLS. |

## Modelo de datos del grafo

### Etiquetas de nodo

`Project`, `Package`, `Folder`, `File`, `Module`, `Class`, `Function`, `Method`, `Interface`, `Enum`, `Type`, `Route`, `Resource`

### Tipos de arista

`CONTAINS_PACKAGE`, `CONTAINS_FOLDER`, `CONTAINS_FILE`, `DEFINES`, `DEFINES_METHOD`, `IMPORTS`, `CALLS`, `HTTP_CALLS`, `ASYNC_CALLS`, `IMPLEMENTS`, `HANDLES`, `USAGE`, `CONFIGURES`, `WRITES`, `MEMBER_OF`, `TESTS`, `USES_TYPE`, `FILE_CHANGES_WITH`

### Nombres cualificados

`get_code_snippet` usa nombres cualificados: `<project>.<path_parts>.<name>`. Usa `search_graph` para descubrirlos primero.

### Cypher soportado (subconjunto de lectura de openCypher)

`query_graph` es un subconjunto de openCypher de solo lectura:

- **Cláusulas**: `MATCH`, `OPTIONAL MATCH`, múltiples `MATCH`, `WHERE`, `WITH` (+ `WITH … WHERE`), `RETURN`, `ORDER BY`, `SKIP`, `LIMIT`, `DISTINCT`, `UNWIND`, `UNION` / `UNION ALL`, `CASE`.
- **Patrones**: nodos etiquetados, alternancia de etiquetas `(n:A|B)`, tipos/dirección de relación, rutas de longitud variable `[*1..3]`, mapas de propiedades en línea.
- **WHERE**: `= <> < <= > >=`, `AND/OR/XOR/NOT`, `IN`, `CONTAINS`, `STARTS WITH`, `ENDS WITH`, `IS [NOT] NULL`, regex `=~`, prueba de etiqueta `n:Label`, y `EXISTS { (n)-[:TYPE]->() }` (existencia de un solo salto — ideal para código muerto, p. ej. `WHERE NOT EXISTS { (f)<-[:CALLS]-() }`).
- **Agregados**: `count` (+`DISTINCT`), `sum`, `avg`, `min`, `max`, `collect`.
- **Funciones**: `labels`, `type`, `id`, `keys`, `properties`; `toLower/toUpper/toString/toInteger/toFloat/toBoolean`; `size`, `length`, `trim/ltrim/rtrim`, `reverse`; `coalesce`, `substring`, `replace`, `left`, `right`.

Cualquier cosa fuera de este subconjunto (cláusulas de escritura/`MERGE`/`CALL`, funciones no soportadas, literales de lista/mapa, comprensiones, funciones de ruta, parámetros) **falla con un error claro `unsupported …`** en lugar de devolver resultados vacíos.

## Ignorar archivos

Por capas: patrones codificados (`.git`, `node_modules`, etc.) → jerarquía de `.gitignore` → `.cbmignore` (específico del proyecto, sintaxis gitignore). Los enlaces simbólicos siempre se omiten.

## Configuración

```bash
codebase-memory-mcp config list                          # mostrar todos los ajustes
codebase-memory-mcp config set auto_index true           # auto-indexar al iniciar sesión
codebase-memory-mcp config set auto_index_limit 50000    # máximo de archivos para auto-indexar
codebase-memory-mcp config reset auto_index              # restablecer al valor por defecto
```

### Variables de entorno

| Variable | Por defecto | Descripción |
|----------|---------|-------------|
| `CBM_CACHE_DIR` | `~/.cache/codebase-memory-mcp` | Sobrescribe el directorio de almacenamiento de la base de datos. Todos los índices de proyectos y la configuración se guardan aquí. |
| `CBM_DIAGNOSTICS` | `false` | Pon `1` o `true` para activar la salida periódica de diagnósticos a `/tmp/cbm-diagnostics-<pid>.json` (solo archivo local; nada se envía a la red). |
| `CBM_DOWNLOAD_URL` | *(releases de GitHub)* | Sobrescribe la URL de descarga para las actualizaciones. Se usa para pruebas o despliegues auto-alojados. |
| `CBM_LOG_LEVEL` | `info` | Establece el nivel mínimo de log. Valores aceptados (sin distinguir mayúsculas): `debug`, `info`, `warn`, `error`, `none` — o sus equivalentes numéricos `0`–`4` según el enum interno. Los logs van a stderr; stdout se reserva para el JSON-RPC de MCP. |
| `CBM_WORKERS` | *(detectado)* | Sobrescribe el número de workers de indexación en paralelo que devuelve `cbm_default_worker_count`. Útil dentro de contenedores donde `sysconf(_SC_NPROCESSORS_ONLN)` reporta las CPUs del host en lugar de la cuota efectiva del cgroup. Rango 1–256; los valores inválidos se ignoran con una advertencia. |
| `CBM_DUMP_VERIFY_MIN_RATIO` | `0.5` | Tras la indexación, compara el conteo de nodos persistidos en SQLite con el conteo del volcado en memoria. Cuando los nodos persistidos caen por debajo de esta fracción de los nodos confirmados (y los confirmados > 50), `index_repository` devuelve `status:"degraded"` en lugar de un silencioso `indexed`. Rango 0–1; pon `0` para desactivar. Los valores inválidos se ignoran con una advertencia. |

```bash
# Guardar los índices en un directorio personalizado
export CBM_CACHE_DIR=~/my-projects/cbm-data
```

## Extensiones de archivo personalizadas

Mapea extensiones de archivo adicionales a lenguajes soportados mediante archivos de configuración JSON. Útil para extensiones específicas de frameworks como `.blade.php` (Laravel) o `.mjs` (módulos ES).

**Por proyecto** (en la raíz de tu repo):
```json
// .codebase-memory.json
{"extra_extensions": {".blade.php": "php", ".mjs": "javascript"}}
```

**Global** (se aplica a todos los proyectos):
```json
// ~/.config/codebase-memory-mcp/config.json  (o $XDG_CONFIG_HOME/...)
{"extra_extensions": {".twig": "html", ".phtml": "php"}}
```

La configuración del proyecto tiene prioridad sobre la global para las extensiones en conflicto. Los valores de lenguaje desconocidos se omiten silenciosamente. Los archivos de configuración ausentes se ignoran.

## Persistencia

Las bases de datos SQLite se guardan en `~/.cache/codebase-memory-mcp/`. Persisten entre reinicios (modo WAL, seguro ACID). Para reiniciar: `rm -rf ~/.cache/codebase-memory-mcp/`.

## Resolución de problemas

| Problema | Solución |
|---------|-----|
| `/mcp` no muestra el servidor | Comprueba que la ruta de `.mcp.json` sea absoluta. Reinicia el agente. Prueba: `echo '{}' \| /path/to/binary` debería producir JSON. |
| `index_repository` falla | Pasa una ruta absoluta: `index_repository(repo_path="/absolute/path")` |
| `trace_path` devuelve 0 resultados | Usa primero `search_graph(name_pattern=".*PartialName.*")` para encontrar el nombre exacto. |
| Las consultas devuelven resultados del proyecto equivocado | Añade el parámetro `project="name"`. Usa `list_projects` para ver los nombres. |
| Binario no encontrado tras la instalación | Añádelo al PATH: `export PATH="$HOME/.local/bin:$PATH"` |
| La UI no carga | Asegúrate de haber descargado la variante `ui` y de ejecutar `--ui=true`. Comprueba `http://localhost:9749`. |

## Hybrid LSP

**Resolución semántica de tipos más allá de tree-sitter.**

Tree-sitter por sí solo da un AST sintáctico. Eso maneja bien el nombrado, la estructura y los sitios de llamada, pero no puede decirte que `user.profile.display_name()` se resuelve a `Profile.display_name` declarado tres módulos más allá — tree-sitter no sigue imports, genéricos, herencia ni tipos de la biblioteca estándar.

codebase-memory-mcp incorpora una **implementación ligera en C de algoritmos de resolución de tipos de lenguajes, estructuralmente inspirada en y compatible con los principales servidores de lenguaje** (tsserver / typescript-go, pyright, gopls, Roslyn, Eclipse JDT, rust-analyzer), incrustada directamente en el binario estático. Sin proceso de servidor de lenguaje, sin configuración por proyecto, sin clave de API. A esta capa la llamamos **Hybrid LSP**: se ejecuta junto a tree-sitter en cada análisis y refina las aristas `CALLS`, `USAGE` y `RESOLVED_CALLS` con información de tipos, de modo que el grafo resultante refleja lo que una "Ir a definición" de un IDE resolvería.

**Lenguajes con Hybrid LSP completo:**

| Lenguaje | Qué maneja |
|----------|-----------------|
| **Python** *(nuevo en v0.7.0)* | imports + recorridos de submódulos con puntos, dataclasses, tipos de retorno `Self`, genéricos, `@property`, patrones de clase `match/case`, `Mapped[T]` de SQLAlchemy 2.0, `BaseModel` de Pydantic, `typing.Annotated` / `ClassVar` / `Final` / `InitVar`, async/await, classmethod/staticmethod, estrechamiento (`isinstance` / `is not None` / walrus), `typing.cast` / `assert_type`, biblioteca estándar común (logging, pathlib, json, functools). Objetivo de ~95% de resolución en código idiomático. |
| **TypeScript / JavaScript / JSX / TSX** | genéricos, despacho de componentes JSX, inferencia JSDoc para JS plano, declaraciones `.d.ts`, re-exportaciones de módulos, encadenamiento de métodos vía propagación del tipo de retorno, overlay por archivo encadenado a un registro compartido entre archivos |
| **PHP** *(nuevo en v0.7.0)* | namespaces, traits, late-static-binding, inferencia PHPDoc, vinculación de parámetros, inferencia de tipo de retorno |
| **C#** *(nuevo en v0.7.0)* | usings globales, namespaces con ámbito de archivo, records (incl. constructores primarios de C# 12), sintaxis de método LINQ, desempaquetado de `async Task<T>` / `ValueTask<T>`, métodos genéricos, despacho `this` / `base`, inferencia de `var`, biblioteca BCL común |
| **Go** *(afinado en v0.7.0)* | registro entre archivos por paquete precompilado, genéricos, structs embebidos, satisfacción de interfaces, resolución de imports consciente del paquete |
| **C / C++** *(afinado en v0.7.0)* | registro entre archivos por lenguaje precompilado y compartido entre C y C++; el lado C maneja macros + cadenas de `typedef` + enlace entre cabecera y fuente; el lado C++ maneja plantillas, namespaces, inferencia de `auto` y resolución de métodos vía jerarquía de clases |
| **Java** *(nuevo en v0.8.0)* | imports (de un tipo, on-demand, estáticos), jerarquías de clases con despacho `this` / `super`, genéricos, anotaciones, coincidencia de sobrecargas por aridad y tipos de parámetro, lambdas / referencias a método vinculadas a interfaces funcionales, inferencia de tipo de campo, biblioteca JDK común |
| **Kotlin** *(nuevo en v0.8.0)* | imports + resolución del mismo paquete, clases / objects / companion objects, funciones de extensión, data classes, desempaquetado de tipos anulables, funciones de ámbito (`let` / `apply` / `run` / `also` / `with`), llamadas infijas, biblioteca estándar común |
| **Rust** *(nuevo en v0.8.0)* | declaraciones `use` + rutas de módulo, bloques `impl` y métodos de trait, campos de struct, genéricos con límites de trait, desazucarado de traits de operador, síntesis de métodos por derive-macro, rutas estáticas UFCS, prelude común de std |

**Arquitectura de dos capas:**

1. **Pasada de tree-sitter** — rápida, sintáctica, se ejecuta para cada uno de los 158 lenguajes. Extrae definiciones, llamadas, imports.
2. **Pasada de Hybrid LSP** — consciente de tipos, se ejecuta por encima de la pasada de tree-sitter por lenguaje. Refina las aristas de llamada usando el grafo de imports más un registro de definiciones por archivo o precompilado entre archivos. Los lenguajes sin una pasada de Hybrid LSP todavía recurren a la resolución textual, así que siempre obtienes *alguna* respuesta.

El resultado es un grafo de conocimiento lo bastante preciso como para impulsar `trace_path` entre paquetes, jerarquías de herencia y llamadas a la biblioteca estándar — sin pagar por un proceso de servidor de lenguaje por proyecto.

## Soporte de lenguajes

158 lenguajes, todos analizados mediante gramáticas tree-sitter incorporadas y compiladas dentro del binario. Medidos contra 64 repositorios open-source reales (de 78 a 49K nodos):

| Nivel | Puntuación | Lenguajes |
|------|-------|-----------|
| **Excelente** (>= 90%) | | Lua, Kotlin, C++, Perl, Objective-C, Groovy, C, Bash, Zig, Swift, CSS, YAML, TOML, HTML, SCSS, HCL, Dockerfile |
| **Bueno** (75-89%) | | Python, TypeScript, TSX, Go, Rust, Java, R, Dart, JavaScript, Erlang, Elixir, Scala, Ruby, PHP, C#, SQL |
| **Funcional** (< 75%) | | OCaml, Haskell |

También soportados (aún no medidos): Ada, Agda, Apex, Assembly (NASM), Astro, AWK, Beancount, BibTeX, Bicep, Bitbake, Blade, Cairo, Cap'n Proto, Clojure, CMake, COBOL, Common Lisp, Crystal, CSV, CUDA, D, Devicetree, Diff, .env, Elm, Emacs Lisp, F#, Fennel, Fish, FORM, Fortran, FunC, GDScript, .gitattributes, .gitignore, Gleam, GLSL, GN, Go module, Go template, GraphQL, Hare, HLSL, Hyprlang, INI, ISPC, Janet, Jinja2, JSDoc, JSON, JSON5, Jsonnet, Julia, Just, Kconfig, KDL, Lean 4, Linker Script, Liquid, LLVM IR, Luau, Magma, Makefile, Markdown, MATLAB, Mermaid, Meson, Move, Nickel, Nim, Nix, Odin, Pascal, Pkl, PO (gettext), Pony, PowerShell, Prisma, .properties, Protobuf, Puppet, PureScript, Racket, Regex, requirements.txt, ReScript, RON, reStructuredText, Scheme, Slang, Smali, Smithy, Solidity, SOQL, SOSL, Squirrel, SSH config, Starlark, Svelte, Sway, SystemVerilog, TableGen, Tcl, Teal, Templ, Thrift, TLA+, Typst, Verilog, VHDL, Vim script, Vue, WGSL, WIT, Wolfram, XML, Zsh.

## Arquitectura

```
src/
  main.c              Punto de entrada (servidor MCP por stdio + CLI + install/update/config)
  mcp/                Servidor MCP (14 herramientas, JSON-RPC 2.0, detección de sesión, auto-indexación)
  cli/                Install/uninstall/update/config (10 agentes, hooks, instrucciones)
  store/              Almacenamiento del grafo en SQLite (nodos, aristas, recorrido, búsqueda, Louvain)
  pipeline/           Indexación multi-pasada (estructura → definiciones → llamadas → enlaces HTTP → config → tests)
  cypher/             Lexer, parser, planner y ejecutor de consultas Cypher
  discover/           Descubrimiento de archivos (.gitignore, .cbmignore, manejo de enlaces simbólicos)
  watcher/            Auto-sincronización en segundo plano (sondeo de git, intervalos adaptativos)
  traces/             Ingesta de trazas en tiempo de ejecución
  ui/                 Servidor HTTP incrustado + visualización de grafo 3D
  foundation/         Abstracciones de plataforma (hilos, sistema de archivos, logging, memoria)
internal/cbm/         Gramáticas tree-sitter incorporadas (158 lenguajes) + motor de extracción de AST
```

## Privacidad y telemetría

**Esta herramienta no contiene telemetría ni "phone-home".** Todo el procesamiento (descubrimiento de archivos, análisis, indexación, embeddings, consultas) ocurre 100% en local. El servidor MCP **no realiza ninguna llamada de red automática** — ni al arrancar, ni en el handshake `initialize`, ni en la primera llamada a una herramienta, ni durante la indexación o la auto-sincronización.

Detalles:

- **Sin comprobación automática de actualizaciones.** Las versiones anteriores lanzaban en el arranque un hilo en segundo plano que contactaba con `api.github.com` para comprobar si había una versión más reciente. Esa comprobación **se ha eliminado por completo** del servidor MCP (en `src/mcp/mcp.c`), por lo que el simple hecho de conectar tu agente ya no revela a GitHub que estás ejecutando la herramienta.
- **Acceso a la red solo bajo petición explícita.** El único comando que accede a la red es `codebase-memory-mcp update`, y solo se ejecuta cuando tú lo invocas a mano. Los scripts de instalación (`install.sh` / `install.ps1` / `scripts/setup*.sh`) **descargan** el binario y `checksums.txt` desde GitHub cuando los ejecutas — son descargas (pull) verificadas con SHA-256, no envían ningún dato sobre ti o tu máquina.
- **Servidor de la UI solo en loopback.** El servidor HTTP opcional de la visualización de grafo se enlaza exclusivamente a `127.0.0.1` y está desactivado por defecto. Sus peticiones son todas del mismo origen hacia el backend local; el frontend `graph-ui` no carga ningún CDN, fuente remota ni SDK de analítica.
- **Diagnósticos solo en local.** `CBM_DIAGNOSTICS` está desactivado por defecto; cuando se activa, solo escribe un archivo JSON en el directorio temporal local — nada se transmite.

Si quieres un binario garantizado sin la comprobación de actualizaciones, [compílalo desde este código fuente](#compilar-desde-el-código-fuente) e instálalo con el subcomando `install` del binario (que no accede a la red), en lugar de descargar un binario precompilado.

## Seguridad

Cada binario publicado se verifica a través de un pipeline multicapa antes de su publicación:

- **VirusTotal** — todos los binarios escaneados por más de 70 motores antivirus (se requieren cero detecciones para publicar)
- **SLSA Level 3** — procedencia criptográfica de la compilación generada por GitHub Actions; verifica con `gh attestation verify <file> --repo DeusData/codebase-memory-mcp`
- **Sigstore cosign** — firmas sin clave en todos los artefactos; los bundles se incluyen en cada versión publicada
- **Sumas de verificación SHA-256** — `checksums.txt` publicado con cada versión; verificado por ambos scripts de instalación antes de la extracción
- **CodeQL SAST** — bloquea el pipeline de publicación si quedan alertas abiertas
- **Cero dependencias de runtime** — sin cadena de suministro transitiva; todas las bibliotecas incorporadas en tiempo de compilación

### Escaneos de VirusTotal de la v0.7.0

| Binario | SHA-256 | VirusTotal |
|--------|---------|-----------|
| `linux-amd64` | `8e12bb2d6ead7f20a6d3...` | [0/72 ✅](https://www.virustotal.com/gui/file/8e12bb2d6ead7f20a6d3bf2be1e51f978c38acce810f0734f510d134b039d152/detection) |
| `linux-arm64` | `10f7136bfbf3950c6b2a...` | [0/72 ✅](https://www.virustotal.com/gui/file/10f7136bfbf3950c6b2a1a950bbf85e88b97ee55ab00b4dfbc2a5e9c2ede8672/detection) |
| `darwin-arm64` | `7062a7408906344bf4f8...` | [0/72 ✅](https://www.virustotal.com/gui/file/7062a7408906344bf4f835e9580048af85d12dd2b7cec0edf869df93ad9a0592/detection) |
| `darwin-amd64` | `28c6d640e1a0ac7bfcab...` | [0/72 ✅](https://www.virustotal.com/gui/file/28c6d640e1a0ac7bfcab5094c2186eced5264a20dcdffcb4455a1b28c5df2171/detection) |
| `windows-amd64` | `9c3ddcf78368fd4fa891...` | [0/72 ✅](https://www.virustotal.com/gui/file/9c3ddcf78368fd4fa89156a553641bf1e03640b4fb6dd29a12c84aa5bc98cd86/detection) |

Los enlaces de escaneo de cada versión también se incluyen automáticamente en las notas de la Release de GitHub.

## Licencia

MIT

# USO — codebase-memory-mcp

Binario local (sin telemetría): `~/.local/bin/codebase-memory-mcp.exe`
En PowerShell: `$env:USERPROFILE\.local\bin\codebase-memory-mcp.exe`

---

## 1. Activar

1. Reinicia VS Code / Claude Code.
2. Escribe `/mcp` → debe salir **codebase-memory-mcp** (14 tools).
3. Prueba: *"Lista los proyectos indexados"*.

---

## 2. Configurar un proyecto

En el agente, lenguaje natural:
```
Indexa este proyecto
```

Auto-indexar siempre (PowerShell):
```powershell
& "$env:USERPROFILE\.local\bin\codebase-memory-mcp.exe" config set auto_index true
& "$env:USERPROFILE\.local\bin\codebase-memory-mcp.exe" config set auto_index_limit 50000
```

Excluir archivos → crea `.cbmignore` en la raíz:
```
dist/
vendor/
*.min.js
```

Compartir índice con el equipo (opcional): se genera `.codebase-memory/graph.db.zst`; confírmalo en git y los demás no reindexan.

---

## 3. Comprimir tokens (lo importante)

Una consulta al grafo reemplaza decenas de grep/lectura (~99% menos tokens).
Hay un hook que ya aumenta `Grep`/`Glob` automáticamente.

Pídeselo así en Claude Code:

| Quieres | Escribe | Tool |
|---|---|---|
| Resumen del proyecto | *"Dame la arquitectura: lenguajes, rutas, hotspots"* | `get_architecture` |
| Quién llama a algo | *"¿Qué llama a `ProcessOrder`? entrante y saliente"* | `trace_path` |
| Encontrar símbolos | *"Busca funciones `.*Handler.*`"* | `search_graph` |
| Código muerto | *"Funciones sin llamadores"* | `query_graph` |
| Impacto de cambios | *"¿Qué afectan mis cambios sin confirmar?"* | `detect_changes` |
| Leer una función | *"Muéstrame el código de `Service.run`"* | `get_code_snippet` |
| Por significado | *"Busca dónde se valida el login"* | `semantic_query` |

**Tip:** al empezar la sesión di una vez:
> *"Para explorar el código usa primero search_graph / trace_path / get_architecture, no leas archivos uno a uno."*

---

## 4. Ver los grafos (UI 3D)

Requiere la **variante con UI** (la estándar instalada no la trae).

Construir la UI una vez:
```bash
# en Git Bash, dentro del repo
cd graph-ui && npm install && npm run build && cd ..
mingw32-make -f Makefile.cbm cbm-with-ui CC=gcc CXX=g++ \
  MIMALLOC_OVERRIDE_DEFINE='-DMI_MALLOC_OVERRIDE=1 -DERROR_COMMITMENT_MINIMUM=635L' \
  GCC_ONLY_FLAGS='-Wno-format-truncation -Wno-unused-result -Wno-stringop-truncation' \
  CFLAGS_EXTRA='-D__USE_MINGW_ANSI_STDIO=1 -D_WIN32_WINNT=0x0600 -Wno-error' -j8
./build/c/codebase-memory-mcp.exe install -y --force   # reinstala la versión con UI
```

Levantarla:
```powershell
& "$env:USERPROFILE\.local\bin\codebase-memory-mcp.exe" --ui=true --port=9749
```
Abre **http://localhost:9749** (solo loopback, sin red externa).

Dejarla activada por defecto:
```powershell
& "$env:USERPROFILE\.local\bin\codebase-memory-mcp.exe" config set ui_enabled true
```

---

## 5. CLI rápida (pruebas)

```powershell
$cbm = "$env:USERPROFILE\.local\bin\codebase-memory-mcp.exe"
& $cbm cli index_repository '{"repo_path":"C:/ruta/al/repo"}'
& $cbm cli list_projects
& $cbm cli search_graph '{"name_pattern":".*Handler.*","label":"Function","project":"<nombre>"}'
& $cbm cli trace_path '{"function_name":"main","direction":"both","project":"<nombre>"}'
```
(El nombre del proyecto sale de `list_projects`. Vía agente no hace falta indicarlo.)

---

## 6. Mantenimiento

```powershell
$cbm = "$env:USERPROFILE\.local\bin\codebase-memory-mcp.exe"
& $cbm config list          # ver ajustes
& $cbm uninstall            # quitar config de todos los agentes
Remove-Item -Recurse -Force "$env:USERPROFILE\.cache\codebase-memory-mcp"   # borrar índices
```

> Privacidad: no hay telemetría ni llamadas de red automáticas. Solo `update` accede a la red y solo si lo ejecutas tú.

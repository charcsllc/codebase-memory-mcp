#!/usr/bin/env bash
# install-local.sh — Compila E instala codebase-memory-mcp desde el código LOCAL.
#                     MULTIPLATAFORMA: Linux, macOS y Windows (Git Bash). Detecta
#                     el SO automáticamente (no necesitas un script aparte por SO).
#
# A diferencia de install.sh (eliminado), NO descarga nada de la red: compila el
# binario desde ESTE código fuente (con la telemetría ya eliminada) y lo instala
# con el subcomando 'install' del propio binario (que tampoco accede a la red).
#
# Uso:
#   ./install-local.sh                # compila el binario estándar e instala
#   ./install-local.sh --with-ui      # compila también la UI 3D (requiere npm)
#   ./install-local.sh --skip-build   # no compila; usa el binario de build/c/
#   ./install-local.sh --dry-run      # muestra qué instalaría, sin tocar nada
#
# Windows: ejecútalo desde Git Bash (usa mingw32-make + gcc de MinGW).
# Linux/macOS: usa scripts/build.sh.

set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"

WITH_UI=false; SKIP_BUILD=false; DRY_RUN=false
for a in "$@"; do case "$a" in
    --with-ui)    WITH_UI=true ;;
    --skip-build) SKIP_BUILD=true ;;
    --dry-run)    DRY_RUN=true ;;
    -h|--help)    grep '^#' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
    *) echo "opción desconocida: $a" >&2; exit 1 ;;
esac; done

OS="$(uname -s)"
TARGET=cbm; [ "$WITH_UI" = true ] && TARGET=cbm-with-ui

build() {
    if [ "$WITH_UI" = true ]; then
        command -v npm >/dev/null || { echo "error: falta npm (necesario para --with-ui)" >&2; exit 1; }
        echo "==> Compilando frontend graph-ui (npm)…"
        ( cd graph-ui && npm install && npm run build )
    fi
    echo "==> Compilando binario local ($TARGET) en $OS…"
    case "$OS" in
        Linux|Darwin)
            # Linux / macOS: compilador oficial del proyecto.
            command -v cc >/dev/null || command -v gcc >/dev/null || command -v clang >/dev/null || {
                echo "error: falta un compilador de C. Linux: 'apt install build-essential zlib1g-dev'; macOS: 'xcode-select --install'" >&2
                exit 1; }
            if [ "$WITH_UI" = true ]; then scripts/build.sh --with-ui; else scripts/build.sh; fi
            ;;
        MINGW*|MSYS*|CYGWIN*)
            # Windows + Git Bash con MinGW antiguo (gcc 8.x): banderas de compatibilidad de cabeceras.
            command -v mingw32-make >/dev/null || { echo "error: falta mingw32-make (instala MinGW-w64)" >&2; exit 1; }
            mingw32-make -f Makefile.cbm "$TARGET" CC=gcc CXX=g++ \
                MIMALLOC_OVERRIDE_DEFINE='-DMI_MALLOC_OVERRIDE=1 -DERROR_COMMITMENT_MINIMUM=635L' \
                GCC_ONLY_FLAGS='-Wno-format-truncation -Wno-unused-result -Wno-stringop-truncation' \
                CFLAGS_EXTRA='-D__USE_MINGW_ANSI_STDIO=1 -D_WIN32_WINNT=0x0600 -Wno-error' \
                -j"$(nproc 2>/dev/null || echo 4)"
            ;;
        *)
            echo "error: sistema operativo no soportado: $OS" >&2; exit 1 ;;
    esac
}

[ "$SKIP_BUILD" = true ] || build

BIN="$ROOT/build/c/codebase-memory-mcp"; [ -f "$BIN.exe" ] && BIN="$BIN.exe"
[ -f "$BIN" ] || { echo "error: binario no encontrado en build/c/ (¿falló la compilación?)" >&2; exit 1; }

echo "==> Instalando desde binario local (sin red): $BIN"
if [ "$DRY_RUN" = true ]; then
    "$BIN" install -y --dry-run
else
    "$BIN" install -y --force
fi

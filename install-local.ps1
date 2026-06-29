# install-local.ps1 — Compila E instala codebase-memory-mcp desde el código LOCAL (Windows).
#
# A diferencia de install.ps1 (eliminado), NO descarga nada de la red: compila el
# binario desde ESTE código fuente (con la telemetría ya eliminada) y lo instala.
# La COMPILACIÓN se hace con Git Bash + MinGW (el build es de tipo Unix-make);
# la instalación y el ajuste de PATH se hacen en PowerShell.
#
# Uso:
#   .\install-local.ps1              # compila el binario estándar e instala
#   .\install-local.ps1 -WithUi      # compila también la UI 3D (requiere npm)
#   .\install-local.ps1 -SkipBuild   # no compila; usa el binario de build\c\
#   .\install-local.ps1 -DryRun      # muestra qué instalaría, sin tocar nada

param([switch]$DryRun, [switch]$SkipBuild, [switch]$WithUi)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Bin  = Join-Path $Root "build\c\codebase-memory-mcp.exe"

# --- 1) Compilar desde el código local (vía Git Bash + MinGW) ---
if (-not $SkipBuild) {
    # IMPORTANTE: usar Git Bash, NO C:\Windows\System32\bash.exe (ese es WSL y
    # compilaría un binario de Linux con el toolchain equivocado).
    $bash = "C:\Program Files\Git\bin\bash.exe"
    if (-not (Test-Path $bash)) {
        $found = (Get-Command bash -ErrorAction SilentlyContinue).Source
        if ($found -and $found -notlike "*System32*") { $bash = $found } else { $bash = $null }
    }
    if (-not $bash) {
        Write-Host "error: se necesita Git Bash para compilar. Instala Git, o usa install-local.sh desde Git Bash." -ForegroundColor Red
        exit 1
    }

    $target = if ($WithUi) { "cbm-with-ui" } else { "cbm" }
    $uiPrefix = if ($WithUi) { "( cd graph-ui && npm install && npm run build ) && " } else { "" }
    $rootUnix = $Root -replace '\\','/'
    $make = "mingw32-make -f Makefile.cbm $target CC=gcc CXX=g++ " +
            "MIMALLOC_OVERRIDE_DEFINE='-DMI_MALLOC_OVERRIDE=1 -DERROR_COMMITMENT_MINIMUM=635L' " +
            "GCC_ONLY_FLAGS='-Wno-format-truncation -Wno-unused-result -Wno-stringop-truncation' " +
            "CFLAGS_EXTRA='-D__USE_MINGW_ANSI_STDIO=1 -D_WIN32_WINNT=0x0600 -Wno-error' -j8"

    Write-Host "==> Compilando desde el codigo local (Git Bash + MinGW)..."
    & $bash -lc "cd '$rootUnix' && $uiPrefix$make"
    if ($LASTEXITCODE -ne 0) { Write-Host "error: fallo la compilacion." -ForegroundColor Red; exit 1 }
}

if (-not (Test-Path $Bin)) {
    Write-Host "error: no existe $Bin (compila sin -SkipBuild)." -ForegroundColor Red
    exit 1
}

# --- 2) Instalar desde el binario local (sin red) ---
Write-Host "==> Instalando desde binario local (sin red): $Bin"
if ($DryRun) {
    & $Bin install -y --dry-run
    exit 0
}
& $Bin install -y --force

# --- 3) Añadir ~/.local/bin al PATH de USUARIO de Windows ---
# (rescatado de install.ps1: el subcomando 'install' solo escribe ~/.profile de Git Bash)
$target = Join-Path $env:USERPROFILE ".local\bin"
$userPath = [Environment]::GetEnvironmentVariable("PATH", "User")
if ($userPath -notlike "*$target*") {
    [Environment]::SetEnvironmentVariable("PATH", "$userPath;$target", "User")
    $env:PATH = "$env:PATH;$target"
    Write-Host "Anadido $target al PATH de usuario de Windows (reinicia la terminal)."
}

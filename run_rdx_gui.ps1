# RDX GUI Launcher Script
# This script sets up the environment and launches rdx_gui.exe

$ErrorActionPreference = "Stop"

Write-Host "=== RDX GUI Launcher ===" -ForegroundColor Cyan

# Set paths
$buildDir = Split-Path -Parent $MyInvocation.MyCommand.Path
if (-not (Test-Path $buildDir)) {
    $buildDir = "C:\Repos\RDX\build"
}

$exePath = Join-Path $buildDir "src\app\Release\rdx_gui.exe"
$qtPath = "C:\Qt\6.10.1\msvc2022_64\bin"

# Check if executable exists
if (-not (Test-Path $exePath)) {
    Write-Host "❌ Error: rdx_gui.exe not found at:" -ForegroundColor Red
    Write-Host "   $exePath" -ForegroundColor Yellow
    Write-Host "`nPlease build the project first:" -ForegroundColor Yellow
    Write-Host "   cd C:\Repos\RDX\build" -ForegroundColor White
    Write-Host "   cmake --build . --config Release" -ForegroundColor White
    exit 1
}

# Check Qt6 DLLs
Write-Host "`nChecking Qt6 dependencies..." -ForegroundColor Yellow
$requiredDlls = @("Qt6Core.dll", "Qt6Gui.dll", "Qt6Qml.dll", "Qt6Quick.dll", "Qt6QuickControls2.dll", "Qt6Widgets.dll")
$missingDlls = @()

foreach ($dll in $requiredDlls) {
    $dllPath = Join-Path $qtPath $dll
    if (-not (Test-Path $dllPath)) {
        $missingDlls += $dll
    }
}

if ($missingDlls.Count -gt 0) {
    Write-Host "❌ Missing Qt6 DLLs:" -ForegroundColor Red
    foreach ($dll in $missingDlls) {
        Write-Host "   $dll" -ForegroundColor Yellow
    }
    Write-Host "`nQt6 path: $qtPath" -ForegroundColor Cyan
    Write-Host "Please verify Qt6 installation." -ForegroundColor Yellow
    exit 1
}

# Add Qt6 to PATH
Write-Host "✅ All dependencies found" -ForegroundColor Green
$env:Path = "$qtPath;$env:Path"

# Launch application
Write-Host "`n🚀 Launching RDX GUI..." -ForegroundColor Cyan
Write-Host "   $exePath" -ForegroundColor Gray

try {
    & $exePath
    Write-Host "`n✅ Application started" -ForegroundColor Green
} catch {
    Write-Host "`n❌ Failed to launch application:" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Yellow
    exit 1
}


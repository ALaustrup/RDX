# RDX Dependency Installation Script for Windows
# This script helps install SQLite3, zstd, and Qt6 dependencies

Write-Host "RDX Dependency Installation Script" -ForegroundColor Cyan
Write-Host "=================================" -ForegroundColor Cyan
Write-Host ""

# Check if vcpkg is available
$vcpkgPath = "C:\vcpkg\vcpkg.exe"
$useVcpkg = $false

if (Test-Path $vcpkgPath) {
    Write-Host "vcpkg found at C:\vcpkg" -ForegroundColor Green
    $useVcpkg = $true
} else {
    Write-Host "vcpkg not found. Checking for Chocolatey..." -ForegroundColor Yellow
    
    # Check for Chocolatey
    if (Get-Command choco -ErrorAction SilentlyContinue) {
        Write-Host "Chocolatey found!" -ForegroundColor Green
        Write-Host ""
        Write-Host "Installing dependencies via Chocolatey..." -ForegroundColor Cyan
        
        # Install SQLite
        Write-Host "Installing SQLite..." -ForegroundColor Yellow
        choco install sqlite -y
        
        # Install zstd (if available)
        Write-Host "Installing zstd..." -ForegroundColor Yellow
        choco install zstd -y 2>$null
        
        Write-Host ""
        Write-Host "Dependencies installed via Chocolatey!" -ForegroundColor Green
        Write-Host "Note: You may need to manually place libraries in extern/ directories" -ForegroundColor Yellow
    } else {
        Write-Host "Chocolatey not found." -ForegroundColor Yellow
        Write-Host ""
        Write-Host "Manual Installation Instructions:" -ForegroundColor Cyan
        Write-Host "1. SQLite3:" -ForegroundColor White
        Write-Host "   Download from: https://www.sqlite.org/download.html" -ForegroundColor Gray
        Write-Host "   Place sqlite3.h in extern/sqlite/include/" -ForegroundColor Gray
        Write-Host "   Place sqlite3.lib in extern/sqlite/lib/" -ForegroundColor Gray
        Write-Host ""
        Write-Host "2. zstd:" -ForegroundColor White
        Write-Host "   Download from: https://github.com/facebook/zstd/releases" -ForegroundColor Gray
        Write-Host "   Place zstd.h in extern/zstd/include/" -ForegroundColor Gray
        Write-Host "   Place zstd.lib in extern/zstd/lib/" -ForegroundColor Gray
        Write-Host ""
        Write-Host "3. Qt6:" -ForegroundColor White
        Write-Host "   Download from: https://www.qt.io/download" -ForegroundColor Gray
        Write-Host "   Install Qt6 with Qt Quick and QML components" -ForegroundColor Gray
    }
}

# Qt6 installation check
Write-Host ""
Write-Host "Qt6 Installation:" -ForegroundColor Cyan
$qt6Path = $env:Qt6_DIR
if ($qt6Path) {
    Write-Host "Qt6 found at: $qt6Path" -ForegroundColor Green
} else {
    Write-Host "Qt6 not found in environment." -ForegroundColor Yellow
    Write-Host "Please install Qt6 from https://www.qt.io/download" -ForegroundColor White
    Write-Host "Make sure to install Qt Quick and QML components." -ForegroundColor White
}

Write-Host ""
Write-Host "Installation check complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "1. Ensure all dependencies are installed" -ForegroundColor White
Write-Host "2. Run: mkdir build && cd build" -ForegroundColor White
Write-Host "3. Run: cmake .." -ForegroundColor White
Write-Host "4. Run: cmake --build ." -ForegroundColor White


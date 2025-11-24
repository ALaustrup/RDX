# Installing Qt6 MSVC for RDX

## Quick Guide

### Step 1: Launch Qt Maintenance Tool

Run:
```powershell
C:\Qt\MaintenanceTool.exe
```

Or find it in Start Menu: "Qt Maintenance Tool"

### Step 2: Add MSVC Components

1. Click **"Add or remove components"**
2. Expand **"Qt"** → **"Qt 6.10.0"**
3. Check one of:
   - ✅ **MSVC 2019 64-bit** (recommended for Visual Studio 2019)
   - ✅ **MSVC 2022 64-bit** (for Visual Studio 2022)
4. Under the selected MSVC version, also check:
   - ✅ **Qt Quick**
   - ✅ **Qt QML**
   - ✅ **Qt Quick Controls 2** (optional but recommended)
5. Click **"Next"** → **"Install"**
6. Wait for installation to complete (may take 10-30 minutes)

### Step 3: Rebuild RDX

After installation, rebuild with the correct Qt path:

```powershell
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake `
         -Dunofficial-sqlite3_DIR=C:\vcpkg\packages\sqlite3_x64-windows\share\unofficial-sqlite3 `
         -DCMAKE_PREFIX_PATH="C:\Qt\6.10.0\msvc2019_64"
cmake --build . --config Release
```

**Note:** Replace `msvc2019_64` with `msvc2022_64` if you installed that version.

### Step 4: Run the GUI

```powershell
.\src\app\Release\rdx_gui.exe
```

## Verification

Check if MSVC version is installed:
```powershell
Test-Path "C:\Qt\6.10.0\msvc2019_64"
# or
Test-Path "C:\Qt\6.10.0\msvc2022_64"
```

## Troubleshooting

### Maintenance Tool Not Found

Download Qt Online Installer from: https://www.qt.io/download

### Installation Fails

- Ensure you have administrator privileges
- Check disk space (Qt6 MSVC requires ~2-3 GB)
- Try installing components one at a time

### CMake Still Can't Find Qt

- Verify the path: `C:\Qt\6.10.0\msvc2019_64\bin\qmake.exe` exists
- Set `CMAKE_PREFIX_PATH` explicitly in CMake
- Check Qt installation in Maintenance Tool


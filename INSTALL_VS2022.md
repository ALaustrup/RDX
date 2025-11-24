# Installing Visual Studio 2022 for RDX

## Why Visual Studio 2022?

MSVC 2019 has a known compiler bug with Qt6.10.1 that causes internal compiler errors. Visual Studio 2022 includes MSVC 2022 compiler which works correctly with Qt6.10.1.

## Installation Steps

### Step 1: Download

1. Go to: https://visualstudio.microsoft.com/downloads/
2. Download **Visual Studio 2022 Community** (free) or **Professional**
3. Run the installer

### Step 2: Select Workloads

During installation, select:

- ✅ **Desktop development with C++**
  - This includes:
    - MSVC v143 - VS 2022 C++ x64/x86 build tools
    - Windows 10/11 SDK
    - CMake tools for Windows
    - C++ core features

### Step 3: Install

1. Click **Install**
2. Wait for installation to complete (15-30 minutes depending on internet speed)
3. Restart your computer if prompted

### Step 4: Verify Installation

After installation, verify MSVC 2022 is available:

```powershell
# Open a new PowerShell window (important - to get updated PATH)
cl.exe
# Should show version 19.3x or higher (MSVC 2022)
```

### Step 5: Rebuild RDX

Once VS 2022 is installed, rebuild RDX:

```powershell
cd C:\Repos\RDX\build

# Clean previous build
Remove-Item CMakeCache.txt -ErrorAction SilentlyContinue
Remove-Item -Recurse CMakeFiles -ErrorAction SilentlyContinue

# Reconfigure with VS 2022
cmake .. -G "Visual Studio 17 2022" -A x64 `
         -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake `
         -Dunofficial-sqlite3_DIR=C:\vcpkg\packages\sqlite3_x64-windows\share\unofficial-sqlite3 `
         -DCMAKE_PREFIX_PATH="C:\Qt\6.10.1\msvc2022_64" `
         -DQt6_DIR="C:\Qt\6.10.1\msvc2022_64\lib\cmake\Qt6"

# Build
cmake --build . --config Release
```

### Step 6: Run the GUI

```powershell
.\src\app\Release\rdx_gui.exe
```

## Alternative: Use Existing VS 2022

If you already have Visual Studio 2022 installed but CMake is using VS 2019:

1. Open **Visual Studio Installer**
2. Ensure "Desktop development with C++" is installed
3. Use the `-G "Visual Studio 17 2022"` generator in CMake

## Troubleshooting

### CMake Still Uses VS 2019

Force CMake to use VS 2022:
```powershell
cmake .. -G "Visual Studio 17 2022" -A x64 ...
```

### Compiler Not Found

1. Open a **Developer Command Prompt for VS 2022**
2. Or use: `& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"`

### Qt Not Found

Verify Qt path:
```powershell
Test-Path "C:\Qt\6.10.1\msvc2022_64\bin\qmake.exe"
```

## Benefits of VS 2022

- ✅ Works with Qt6.10.1 (no compiler bugs)
- ✅ Better C++20 support
- ✅ Improved performance
- ✅ Latest compiler optimizations
- ✅ Better IDE integration


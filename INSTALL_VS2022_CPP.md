# Installing C++ Tools in Visual Studio 2022

## Issue

Visual Studio 2022 is installed, but the **C++ development tools** are missing. This is required to build RDX.

## Solution

### Step 1: Open Visual Studio Installer

- Open **Start Menu**
- Search for "Visual Studio Installer"
- Click to open

### Step 2: Modify VS 2022 Installation

1. Find **"Visual Studio 2022 Community"** (or your edition)
2. Click **"Modify"** button

### Step 3: Select C++ Workload

1. In the **Workloads** tab, check:
   - ✅ **Desktop development with C++**

2. Under **Individual components**, ensure these are checked:
   - ✅ MSVC v143 - VS 2022 C++ x64/x86 build tools (Latest)
   - ✅ Windows 10/11 SDK (latest version)
   - ✅ C++ CMake tools for Windows
   - ✅ C++ core features

### Step 4: Install

1. Click **"Modify"** button at bottom right
2. Wait for installation to complete (10-20 minutes)
3. **Restart your computer** if prompted

### Step 5: Verify Installation

After installation, verify C++ tools are available:

```powershell
# Check for MSVC compiler
& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cl.exe
# Should show MSVC version 19.3x or higher
```

### Step 6: Rebuild RDX

Once C++ tools are installed:

```powershell
cd C:\Repos\RDX\build
Remove-Item CMakeCache.txt -ErrorAction SilentlyContinue
Remove-Item -Recurse CMakeFiles -ErrorAction SilentlyContinue

# Configure
cmake .. -G "Visual Studio 17 2022" -A x64 `
         -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake `
         -Dunofficial-sqlite3_DIR=C:\vcpkg\packages\sqlite3_x64-windows\share\unofficial-sqlite3 `
         -DCMAKE_PREFIX_PATH="C:\Qt\6.10.1\msvc2022_64" `
         -DQt6_DIR="C:\Qt\6.10.1\msvc2022_64\lib\cmake\Qt6"

# Build
cmake --build . --config Release
```

## Quick Check

Verify C++ tools are installed:

```powershell
Test-Path "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC"
# Should return True
```

If it returns `False`, the C++ workload is not installed.


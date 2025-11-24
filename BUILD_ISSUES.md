# Build Issues and Solutions

## MSVC 2019 + Qt6.10.1 Compiler Error

### Issue
When building the GUI with MSVC 2019 (Visual Studio 2019 Build Tools), you may encounter:

```
fatal error C1001: Internal compiler error.
C:/Qt/6.10.1/msvc2022_64/include\QtCore/qrangemodel_impl.h(897,54)
```

This is a known bug in MSVC 2019 compiler when processing Qt6.10.1 template code.

### Solutions

#### Option 1: Use Visual Studio 2022 (Recommended)
Install Visual Studio 2022 which includes MSVC 2022 compiler that works with Qt6.10.1:

1. Download Visual Studio 2022: https://visualstudio.microsoft.com/downloads/
2. Install with "Desktop development with C++" workload
3. Rebuild RDX - it will automatically use MSVC 2022

#### Option 2: Use MinGW Compiler
Since you have Qt6.10.0 MinGW installed, you can build with MinGW:

```powershell
# Install MinGW-w64 if not already installed
# Then configure with MinGW:
cmake .. -G "MinGW Makefiles" `
         -DCMAKE_PREFIX_PATH="C:\Qt\6.10.0\mingw_64" `
         -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake `
         -Dunofficial-sqlite3_DIR=C:\vcpkg\packages\sqlite3_x64-windows\share\unofficial-sqlite3

cmake --build . --config Release
```

#### Option 3: Use Qt6.9.x or Wait for Qt6.10.2
- Install Qt6.9.x MSVC version (more stable with MSVC 2019)
- Or wait for Qt6.10.2 which may fix the compiler issue

#### Option 4: Disable GUI Temporarily
Build only the core library and tests:

```powershell
cmake .. -DBUILD_GUI=OFF
cmake --build . --config Release
```

### Current Status

✅ **Core Library**: Builds successfully with MSVC 2019
✅ **Tests**: Build successfully with MSVC 2019  
❌ **GUI**: Requires MSVC 2022 or MinGW compiler

### Verification

Check your MSVC version:
```powershell
cl.exe
# Look for version number in output
```

MSVC 2019 = version 19.2x
MSVC 2022 = version 19.3x or higher


# Building RDX with Developer Command Prompt

## Why Use Developer Command Prompt?

CMake needs the Visual Studio environment variables set up correctly. The Developer Command Prompt does this automatically.

## Steps

### Step 1: Open Developer Command Prompt

1. Open **Start Menu**
2. Search for: **"Developer Command Prompt for VS 2022"**
3. Click to open (it's a special command prompt with VS environment)

### Step 2: Navigate to Build Directory

```cmd
cd C:\Repos\RDX\build
```

### Step 3: Clean Previous Build

```cmd
del CMakeCache.txt
rmdir /s /q CMakeFiles
rmdir /s /q src
rmdir /s /q tests
```

### Step 4: Configure CMake

```cmd
cmake .. -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake ^
  -Dunofficial-sqlite3_DIR=C:\vcpkg\packages\sqlite3_x64-windows\share\unofficial-sqlite3 ^
  -DCMAKE_PREFIX_PATH=C:\Qt\6.10.1\msvc2022_64 ^
  -DQt6_DIR=C:\Qt\6.10.1\msvc2022_64\lib\cmake\Qt6
```

### Step 5: Build

```cmd
cmake --build . --config Release
```

### Step 6: Run GUI

```cmd
src\app\Release\rdx_gui.exe
```

## Alternative: If VS 2022 Still Not Found

If CMake still can't find VS 2022, try:

1. **Restart your computer** (VS installations often require this)
2. **Verify installation** in Visual Studio Installer
3. **Check PATH** - VS 2022 should add itself to system PATH

## Quick Check

In Developer Command Prompt, verify compiler:

```cmd
cl.exe
```

Should show MSVC version 19.3x (VS 2022) not 19.2x (VS 2019).


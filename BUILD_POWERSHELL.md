# Building RDX from PowerShell

## Quick Build (If VS 2022 is Detected)

If you're in PowerShell and VS 2022 is properly installed:

```powershell
cd C:\Repos\RDX\build
Remove-Item CMakeCache.txt -ErrorAction SilentlyContinue
cmake .. -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake `
  -Dunofficial-sqlite3_DIR=C:\vcpkg\packages\sqlite3_x64-windows\share\unofficial-sqlite3 `
  -DCMAKE_PREFIX_PATH=C:\Qt\6.10.1\msvc2022_64 `
  -DQt6_DIR=C:\Qt\6.10.1\msvc2022_64\lib\cmake\Qt6
cmake --build . --config Release
```

## If VS 2022 Not Found

**Recommended: Use Developer Command Prompt**

See `BUILD_WITH_DEV_CMD.md` for instructions.

**Alternative: Set VS Environment Manually**

```powershell
# Find VS 2022 installation
$vsPath = "C:\Program Files\Microsoft Visual Studio\2022\Community"
if (Test-Path "$vsPath\VC\Auxiliary\Build\vcvars64.bat") {
    # Source the environment
    cmd /c "`"$vsPath\VC\Auxiliary\Build\vcvars64.bat`" && set" | ForEach-Object {
        if ($_ -match '^([^=]+)=(.*)$') {
            [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process')
        }
    }
    # Now run cmake
    cmake .. -G "Visual Studio 17 2022" -A x64
}
```

## Troubleshooting

### Error: "Generator Visual Studio 17 2022 could not find any instance"

**Solution 1: Restart Computer**
VS installations often require a restart to be fully detected.

**Solution 2: Use Developer Command Prompt**
This automatically sets up the VS environment. See `BUILD_WITH_DEV_CMD.md`.

**Solution 3: Verify Installation**
1. Open Visual Studio Installer
2. Verify "Desktop development with C++" is installed
3. Check installation path matches what CMake expects

### Error: "The filename, directory name, or volume label syntax is incorrect"

This usually means:
- Paths have incorrect quotes or escaping
- Using wrong line continuation character (`^` for CMD, `` ` `` for PowerShell)
- Paths contain spaces without proper quoting

**Fix**: Use single-line command or proper PowerShell backtick (`` ` ``) for line continuation.


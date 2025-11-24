# Building RDX

This guide provides detailed instructions for building RDX on different platforms.

## Prerequisites

- **CMake 3.20+**: Required for building
- **C++20 Compiler**: 
  - Windows: MSVC 2019+ or MinGW-w64
  - Linux: GCC 10+ or Clang 12+
  - macOS: Clang 12+ (Xcode 12+)
- **Qt6**: Core, Quick, and QML components
- **SQLite3**: Development libraries
- **zstd**: Compression library

## Platform-Specific Instructions

### Windows

#### Option 1: Using vcpkg (Recommended)

1. Install vcpkg:
```powershell
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

2. Install dependencies:
```powershell
C:\vcpkg\vcpkg install sqlite3:x64-windows zstd:x64-windows
```

3. Install Qt6:
   - Download from https://www.qt.io/download
   - Install Qt6 with Qt Quick and QML components
   - Note the installation path

4. Configure and build:
```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc2019_64
cmake --build . --config Release
```

#### Option 2: Manual Installation

1. Download SQLite3 from https://www.sqlite.org/download.html
   - Extract `sqlite3.h` to `extern/sqlite/include/`
   - Extract `sqlite3.lib` to `extern/sqlite/lib/`

2. Download zstd from https://github.com/facebook/zstd/releases
   - Extract `zstd.h` to `extern/zstd/include/`
   - Extract `zstd.lib` to `extern/zstd/lib/`

3. Install Qt6 from https://www.qt.io/download

4. Build:
```powershell
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc2019_64
cmake --build . --config Release
```

### Linux

#### Debian/Ubuntu

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-declarative-dev \
    libsqlite3-dev \
    libzstd-dev \
    pkg-config

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

#### RHEL/CentOS/Fedora

```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    qt6-qtbase-devel \
    qt6-qtdeclarative-devel \
    sqlite-devel \
    libzstd-devel \
    pkgconfig

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

#### Arch Linux

```bash
sudo pacman -S --needed \
    base-devel \
    cmake \
    qt6-base \
    qt6-declarative \
    sqlite \
    zstd

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install qt@6 sqlite zstd cmake

# Build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
cmake --build .
```

## Build Options

### CMake Options

- `BUILD_TESTS=ON` (default): Build test suite
- `BUILD_GUI=ON` (default): Build Qt6 GUI application
- `CMAKE_BUILD_TYPE`: `Release`, `Debug`, or `RelWithDebInfo`

### Example: Build without GUI

```bash
cmake .. -DBUILD_GUI=OFF
```

### Example: Debug Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## Running Tests

After building:

```bash
cd build
ctest --output-on-failure
```

Or run individual tests:

```bash
./test_schemas
./test_lcm
./test_roundtrip
./test_rdx_end_to_end
```

## Running the GUI

After building:

```bash
cd build
./rdx_gui  # Linux/macOS
# or
.\rdx_gui.exe  # Windows
```

## Troubleshooting

### SQLite3 Not Found

- **Windows**: Ensure SQLite3 is in `extern/sqlite/` or use vcpkg
- **Linux**: Install `libsqlite3-dev` package
- **macOS**: Install via Homebrew: `brew install sqlite`

### Qt6 Not Found

- Set `CMAKE_PREFIX_PATH` to your Qt6 installation directory
- Ensure Qt6 Quick and QML components are installed
- On macOS: Use `$(brew --prefix qt@6)` as prefix path

### zstd Not Found

- **Windows**: Place in `extern/zstd/` or use vcpkg
- **Linux**: Install `libzstd-dev` package
- **macOS**: Install via Homebrew: `brew install zstd`

### Compilation Errors

- Ensure C++20 support is enabled
- Check compiler version (GCC 10+, Clang 12+, MSVC 2019+)
- Verify all dependencies are properly installed

## Cross-Compilation

For cross-compilation, set the appropriate CMake toolchain file:

```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake
```

## IDE Integration

### Visual Studio Code

1. Install CMake Tools extension
2. Open the RDX directory
3. Configure CMake (Ctrl+Shift+P → "CMake: Configure")
4. Build (Ctrl+Shift+P → "CMake: Build")

### Visual Studio

1. Open the RDX directory in Visual Studio
2. Visual Studio will detect CMakeLists.txt
3. Build via Build → Build All

### Qt Creator

1. Open `CMakeLists.txt` in Qt Creator
2. Configure the project
3. Build and run


# RDX - Schema-Aware Compression Engine & Archive Format

[![CI](https://github.com/ALaustrup/RDX/actions/workflows/ci.yml/badge.svg)](https://github.com/ALaustrup/RDX/actions/workflows/ci.yml)
[![CodeQL](https://github.com/ALaustrup/RDX/actions/workflows/codeql.yml/badge.svg)](https://github.com/ALaustrup/RDX/actions/workflows/codeql.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++20](https://img.shields.io/badge/C++-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Qt6](https://img.shields.io/badge/Qt-6-green.svg)](https://www.qt.io/)

RDX is a next-generation compression system that uses **constraint-aware, schema-driven, corpus-learning** architecture to achieve superior compression ratios for structured data formats.

## Overview

RDX differs from traditional compression algorithms by:

- **Schema-Driven Compression**: Uses file-type-specific schemas (PE32, JSON, CSV, logs, configs) to avoid encoding redundant metadata
- **Constraint-Aware Encoding**: Leverages field relationships (length_of, checksum_of, offset_of) to reconstruct exact bytes via canonical rebuilder
- **Lifetime Corpus Model (LCM)**: Maintains a SQLite database that learns from all previously compressed data on the machine
- **File-Type-Aware Parsers**: Lifts input into higher-level structures (tokens, fields, records, sections) before compression
- **Graceful Fallback**: Uses strong residual compression (zstd) when structure doesn't provide gains

## Features

- **Multiple File Format Support**: PE32/PE64 executables, JSON, CSV, log files, INI/config files, chunked binary, and unstructured binary
- **Qt6 GUI Application**: Modern, cross-platform GUI for compression/decompression operations
- **Corpus Learning**: LCM tracks file types, schemas, chunks, and statistics to improve compression over time
- **Professional Architecture**: Clean separation between core library and GUI, reusable components
- **Production-Ready**: Comprehensive error handling, logging, and test suite

## Building

### Prerequisites

- CMake 3.20 or later
- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- Qt6 (Core, Quick, QML)
- SQLite3
- zstd library

#### Quick Install (Automated)

We provide installation scripts for convenience:

**Windows (PowerShell):**
```powershell
.\scripts\install-dependencies-windows.ps1
```

**Linux:**
```bash
chmod +x scripts/install-dependencies-linux.sh
./scripts/install-dependencies-linux.sh
```

**macOS:**
```bash
chmod +x scripts/install-dependencies-macos.sh
./scripts/install-dependencies-macos.sh
```

#### Installing Dependencies (Manual)

**Windows:**
- Install Qt6 from https://www.qt.io/download
- Install SQLite3: Download from https://www.sqlite.org/download.html or use vcpkg: `vcpkg install sqlite3`
- Install zstd: Download from https://github.com/facebook/zstd/releases or use vcpkg: `vcpkg install zstd`
- Alternatively, place SQLite3 and zstd in `extern/sqlite/` and `extern/zstd/` directories (see `extern/README.md`)

**Linux:**
```bash
sudo apt-get install qt6-base-dev qt6-declarative-dev libsqlite3-dev libzstd-dev
```

**macOS:**
```bash
brew install qt@6 sqlite zstd
```

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Build Options

- `BUILD_TESTS=ON` (default): Build test suite
- `BUILD_GUI=ON` (default): Build Qt6 GUI application

## Quick Start

See [QUICKSTART.md](QUICKSTART.md) for a quick guide to get started.

## Usage

### GUI Application

Run the GUI application:

```bash
./rdx_gui  # Linux/macOS
.\rdx_gui.exe  # Windows
```

The GUI provides:
- **Compress Files/Folders**: Select files to compress into `.rdx` archives
- **Decompress Archive**: Extract files from `.rdx` archives
- **Corpus Dashboard**: View LCM statistics (files tracked, top schemas, file types)
- **Settings**: Configure compression level and LCM path

### Command Line (Future)

Command-line interface coming in future releases.

## Architecture

RDX consists of several key components:

1. **LCM (Lifetime Corpus Model)**: SQLite database tracking all compressed files, chunks, schemas, and statistics
2. **Schema System**: Defines structure and constraints for different file types
3. **Parsers**: File-type-aware front-ends that parse input into structured representations
4. **Compression Engine**: Encodes structured data and residual streams
5. **RDX Container Format**: Binary archive format storing compressed files with metadata
6. **Qt6 GUI**: User interface for compression/decompression operations

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed architecture documentation.

## Documentation

- [Architecture Overview](docs/ARCHITECTURE.md)
- [LCM Schema](docs/LCM_SCHEMA.md)
- [Schema DSL](docs/SCHEMA_DSL.md)
- [RDX Format Specification](docs/RDX_FORMAT.md)

## Testing

Run the test suite:

```bash
cd build
ctest
```

Or run individual tests:

```bash
./test_schemas
./test_lcm
./test_roundtrip
./test_rdx_end_to_end
```

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please ensure:

- Code follows C++20 best practices
- All tests pass
- Documentation is updated
- Code is formatted consistently

## Roadmap

- [ ] Enhanced structural encoding with arithmetic coding
- [ ] Generator phase for blob-like regions (bounded CPU/time)
- [ ] Command-line interface
- [ ] Additional file format parsers
- [ ] Performance optimizations
- [ ] Cross-platform packaging

## Acknowledgments

RDX is inspired by the CAC-Prime architecture and constraint-aware compression principles.


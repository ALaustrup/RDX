# RDX Quick Start Guide

Get up and running with RDX in minutes!

## Installation

### Step 1: Install Dependencies

**Windows:**
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

See [BUILD.md](BUILD.md) for detailed installation instructions.

### Step 2: Build RDX

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Step 3: Run the GUI

```bash
# Linux/macOS
./rdx_gui

# Windows
.\rdx_gui.exe
```

## Basic Usage

### Compressing Files

1. Launch the RDX GUI
2. Click "Compress Files"
3. Select one or more files to compress
4. Choose output location for the `.rdx` archive
5. Click "OK" to start compression

The GUI will show:
- Compression progress
- Compression ratio
- Schema used for each file
- Job status (Queued/Running/Done/Failed)

### Decompressing Archives

1. Click "Decompress" in the GUI
2. Select a `.rdx` archive file
3. Choose output directory
4. Click "OK" to extract files

### Viewing Corpus Statistics

1. Click "Corpus Dashboard" in the GUI
2. View:
   - Total files tracked
   - Total corpus size
   - Top schemas by usage
   - Top file types

## Command Line Usage (Future)

Command-line interface coming in future releases. For now, use the GUI.

## Example: Compressing Different File Types

RDX automatically detects and uses appropriate schemas:

- **PE32 executables** (`.exe`, `.dll`): Uses PE32 schema
- **JSON files** (`.json`): Uses JSON schema
- **CSV files** (`.csv`, `.tsv`): Uses CSV schema
- **Log files** (`.log`): Uses log line schema
- **Config files** (`.ini`, `.cfg`): Uses KV config schema
- **Other files**: Uses unstructured binary schema

## Understanding Compression Results

After compression, you'll see:

- **Original Size**: Size of the original file
- **Compressed Size**: Size of the `.rdx` archive entry
- **Compression Ratio**: Percentage of original size
- **Schema Used**: Which schema was applied

The LCM (Lifetime Corpus Model) learns from each compression:
- Similar files compress better over time
- Schema usage statistics improve
- Chunk fingerprints enable better matching

## Tips for Best Compression

1. **Compress similar files together**: The LCM learns patterns
2. **Use appropriate file types**: RDX works best with structured formats
3. **Let the corpus grow**: Compression improves as more files are processed
4. **Check the dashboard**: Monitor which schemas work best

## Troubleshooting

### Build Fails: SQLite3 Not Found

- **Windows**: Use vcpkg or place SQLite3 in `extern/sqlite/`
- **Linux**: Install `libsqlite3-dev`
- **macOS**: Run `brew install sqlite`

### Build Fails: Qt6 Not Found

- Set `CMAKE_PREFIX_PATH` to your Qt6 installation
- Ensure Qt6 Quick and QML are installed
- On macOS: Use `$(brew --prefix qt@6)`

### GUI Won't Start

- Check Qt6 installation
- Verify all dependencies are installed
- Check console for error messages

## Next Steps

- Read [ARCHITECTURE.md](docs/ARCHITECTURE.md) to understand the system design
- Check [LCM_SCHEMA.md](docs/LCM_SCHEMA.md) to learn about the corpus model
- Review [RDX_FORMAT.md](docs/RDX_FORMAT.md) for archive format details
- Explore the source code in `src/`

## Contributing

Contributions welcome! See the main [README.md](README.md) for details.

## Support

- **Issues**: https://github.com/ALaustrup/RDX/issues
- **Discussions**: https://github.com/ALaustrup/RDX/discussions


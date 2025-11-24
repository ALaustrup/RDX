# RDX Architecture

## High-Level Overview

RDX is a schema-aware compression system that combines:

1. **Schema-Driven Parsing**: File-type-specific parsers lift input into structured representations
2. **Constraint-Aware Encoding**: Field relationships (length_of, checksum_of, offset_of) enable exact reconstruction
3. **Lifetime Corpus Model (LCM)**: SQLite database learns from all compressed data
4. **Residual Compression**: Strong codec (zstd) for unstructured portions
5. **RDX Container Format**: Binary archive format with index and blocks

## Component Architecture

### Core Library (`src/core/`)

#### LCM (Lifetime Corpus Model)

**Location**: `src/core/lcm/LCMManager.h/cpp`

The LCM maintains a SQLite database tracking:
- **File Index**: All files ever compressed (content hash, path hash, size, type, schema)
- **Chunk Index**: File chunks with fingerprints for similarity matching
- **Schema Registry**: All schemas with usage statistics
- **File Types**: Detected file types and their signatures
- **Token Profiles**: Statistical models for different file types
- **Bundles**: Groups of files compressed together

**Database Location**:
- Windows: `%PROGRAMDATA%\RDX\LCM\lcm.db`
- Linux: `$XDG_DATA_HOME/rdx/lcm.db` or `$HOME/.local/share/rdx/lcm.db`

#### Schema System

**Location**: `src/core/schemas/`

- **SchemaDefinition**: C++ representation of schemas with fields, constraints, and metadata
- **SchemaRegistry**: Manages built-in schemas and loads custom schemas from LCM

**Supported Schemas**:
- `PE32`: Windows Portable Executable (32/64-bit)
- `JSON_GENERIC`: JSON documents
- `LOG_LINE`: Line-oriented log files
- `CSV_SIMPLE`: CSV/TSV files
- `KV_CONFIG`: INI/properties files
- `CHUNKED_BINARY`: TLV-like binary formats
- `UNSTRUCTURED_BINARY`: Fallback for unknown formats

#### Parsers

**Location**: `src/core/schemas/parsers/`

Each parser implements `ISchemaParser` interface:
- `canParse()`: Determines if parser applies to file type/prefix
- `parse()`: Parses file into `ParsedRepresentation` (structured fields)
- `getSchema()`: Returns associated schema definition

Parsers produce intermediate representations with:
- Structured fields (integers, strings, bytes, records, arrays)
- Constraint graph (field dependencies)
- Metadata

#### File Type Detection

**Location**: `src/core/detectors/FileTypeDetector.h/cpp`

Uses:
- File extension matching
- Magic byte detection
- Returns `DetectedFileType` with ID, name, and signature

#### Compression Engine

**Location**: `src/core/compression/CompressionEngine.h/cpp`

Workflow:
1. Detect file type
2. Select appropriate parser
3. Parse file into structured representation
4. Query LCM for priors (schema stats, token profiles)
5. Encode structural metadata
6. Compress residual data with zstd
7. Record chunks and schema usage in LCM

#### Decompression Engine

**Location**: `src/core/decompression/DecompressionEngine.h/cpp`

Workflow:
1. Read schema ID from RDX entry
2. Load schema definition
3. Decode structural stream
4. Rebuild constraint graph
5. Decompress residual stream
6. Reconstruct exact bytes using canonical rebuilder

#### RDX Container Format

**Location**: `src/core/container/`

- **RDXWriter**: Creates `.rdx` archives
- **RDXReader**: Reads and extracts from `.rdx` archives

Format structure:
```
[RDX_MAGIC][VERSION][FLAGS][INDEX_OFFSET]
[BLOCK_0]
[BLOCK_1]
...
[INDEX_BLOCK]
```

Each block contains:
- Block header (magic, sizes, schema ID, file type ID)
- Compressed structural stream
- Compressed residual stream

### GUI Application (`src/app/`)

#### View Models

- **JobViewModel**: Manages compression/decompression job list (QAbstractListModel)
- **LCMStatsViewModel**: Exposes LCM statistics to QML

#### Controllers

- **CompressionController**: Orchestrates compression/decompression operations

#### QML UI

- **RdxMainWindow.qml**: Main window with navigation
- **JobListView.qml**: List of active/past jobs
- **FileChooserView.qml**: File selection dialog
- **CorpusDashboardView.qml**: LCM statistics display
- **SettingsView.qml**: Configuration UI

## Data Flow

### Compression Flow

```
Input File
  ↓
FileTypeDetector → DetectedFileType
  ↓
SchemaRegistry → SchemaDefinition
  ↓
ISchemaParser → ParsedRepresentation
  ↓
CompressionEngine
  ├─→ Encode Structure → ByteBuffer (structStream)
  ├─→ Extract Residual → ByteBuffer (residualStream)
  └─→ Record in LCM
  ↓
RDXWriter → .rdx Archive
```

### Decompression Flow

```
.rdx Archive
  ↓
RDXReader → RDXEntry
  ↓
DecompressionEngine
  ├─→ Load Schema from LCM
  ├─→ Decode Structure
  └─→ Decompress Residual
  ↓
Canonical Rebuilder → Exact Bytes
  ↓
Output File
```

## Error Handling

- **Parser Failures**: Fallback to unstructured binary parser
- **Compression Errors**: Record in job status, continue with other files
- **LCM Errors**: Log and continue (non-fatal)
- **RDX Format Errors**: Validate magic numbers and structure, throw exceptions

## Thread Safety

- **LCM**: SQLite handles concurrent access (readers don't block writers)
- **CompressionEngine**: Not thread-safe (create per-thread instances)
- **GUI**: All core operations run on worker threads, UI updates via signals/slots

## Performance Considerations

- **LCM Queries**: Use prepared statements for hot paths
- **Chunk Indexing**: Indexed on `chunk_hash` and `chunk_fingerprint` for fast lookups
- **Schema Caching**: Schemas loaded once and cached in SchemaRegistry
- **ZSTD Compression**: Configurable level (default: 3)

## Future Enhancements

- **Generator Phase**: Bounded CPU/time generators for blob-like regions
- **Arithmetic Coding**: Replace simplified encoding with proper arithmetic coder
- **Parallel Compression**: Multi-threaded compression for large files
- **Incremental Updates**: Update existing archives without full recompression


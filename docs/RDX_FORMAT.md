# RDX Archive Format Specification

## Overview

The RDX format is a binary archive format for storing compressed files with schema-aware metadata.

## File Structure

```
RDX Archive:
  [Header]
  [Block 0]
  [Block 1]
  ...
  [Block N]
  [Index]
```

## Header

The header is located at the beginning of the file (offset 0).

| Offset | Size | Type | Description |
|--------|------|------|-------------|
| 0 | 4 | uint32_t | Magic number: `0x52445801` ("RDX" + version) |
| 4 | 2 | uint16_t | Format version (currently 1) |
| 6 | 2 | uint16_t | Flags (reserved, currently 0) |
| 8 | 8 | int64_t | Offset to index block |

**Total Header Size**: 16 bytes

## Block Structure

Each block represents one compressed file.

### Block Header

| Offset | Size | Type | Description |
|--------|------|------|-------------|
| 0 | 4 | uint32_t | Block magic: `0x424C4B01` ("BLK" + version) |
| 4 | 4 | uint32_t | Block header size (in bytes) |
| 8 | 4 | int32_t | Schema ID |
| 12 | 4 | int32_t | File type ID |
| 16 | 8 | int64_t | Original file size |
| 24 | 8 | int64_t | Compressed structural stream size |
| 32 | 8 | int64_t | Compressed residual stream size |
| 40 | 2 | uint16_t | Flags (reserved, currently 0) |

**Total Block Header Size**: 42 bytes (minimum)

### Block Data

After the block header:

1. **Compressed Structural Stream**: `compressed_struct_size` bytes
2. **Compressed Residual Stream**: `compressed_residual_size` bytes

## Index Block

The index block is located at the offset specified in the header.

### Index Header

| Offset | Size | Type | Description |
|--------|------|------|-------------|
| 0 | 4 | uint32_t | Number of entries |

### Index Entries

Each entry describes one file in the archive:

| Offset | Size | Type | Description |
|--------|------|------|-------------|
| 0 | 4 | uint32_t | File name length (N) |
| 4 | N | char[] | File name (UTF-8, null-terminated) |
| 4+N | 8 | int64_t | Original file size |
| 12+N | 8 | int64_t | Compressed structural stream size |
| 20+N | 8 | int64_t | Compressed residual stream size |
| 28+N | 4 | int32_t | Schema ID |
| 32+N | 4 | int32_t | File type ID |
| 36+N | 8 | int64_t | Block offset (from start of file) |
| 44+N | 8 | int64_t | Block size (total) |

## Endianness

All multi-byte integers are stored in **little-endian** format.

## Magic Numbers

- **RDX Magic**: `0x52445801` (ASCII "RDX" + version byte 0x01)
- **Block Magic**: `0x424C4B01` (ASCII "BLK" + version byte 0x01)

## Versioning

### Format Version 1

- Current format version
- 16-byte header
- 42-byte block headers
- Index at end of file

### Future Compatibility

- New format versions will increment the version byte in magic numbers
- Readers should check version and handle unsupported versions gracefully
- Backward compatibility: older readers may skip unknown blocks

## Integrity Checks

### Magic Number Validation

Readers must validate:
1. RDX magic at offset 0
2. Block magic at start of each block

### Size Validation

Readers should validate:
1. Index offset is within file bounds
2. Block offsets are within file bounds
3. Block sizes don't exceed file bounds
4. Sum of block sizes matches file size

## Compression Streams

### Structural Stream

Contains:
- Schema ID
- Encoded field values
- Constraint graph
- Metadata

Currently encoded as simplified JSON-like structure. Future versions will use arithmetic coding.

### Residual Stream

Contains:
- Unstructured data not captured by schema
- Compressed with zstd (level 3 by default)

## Example Layout

```
Offset 0x0000: [RDX Header - 16 bytes]
Offset 0x0010: [Block 0 Header - 42 bytes]
Offset 0x003A: [Block 0 Structural Stream - variable]
Offset 0xXXXX: [Block 0 Residual Stream - variable]
Offset 0xYYYY: [Block 1 Header - 42 bytes]
...
Offset 0xZZZZ: [Index Block]
  - Entry count (4 bytes)
  - Entry 0 (variable)
  - Entry 1 (variable)
  ...
```

## Reading an RDX Archive

1. Read header at offset 0
2. Validate magic number
3. Read index offset
4. Seek to index offset
5. Read entry count
6. For each entry:
   - Read file name and metadata
   - Seek to block offset
   - Read block header
   - Read structural and residual streams
   - Decompress using DecompressionEngine

## Writing an RDX Archive

1. Write header (with placeholder index offset)
2. For each file:
   - Compress using CompressionEngine
   - Write block header
   - Write structural stream
   - Write residual stream
   - Record entry in index
3. Write index block
4. Update index offset in header

## Error Handling

Readers should handle:
- **Invalid magic**: Not an RDX file
- **Unsupported version**: Cannot read this format version
- **Corrupted index**: Index offset invalid or entries malformed
- **Missing blocks**: Block offset beyond file end
- **Decompression errors**: zstd decompression fails

## Future Enhancements

- **Encryption**: Optional encryption of streams
- **Compression**: Optional compression of index
- **Checksums**: Per-block checksums for integrity
- **Metadata**: Extended metadata in index entries
- **Streaming**: Support for streaming compression/decompression


# Lifetime Corpus Model (LCM) Database Schema

The LCM is a SQLite database that tracks all compressed files, chunks, schemas, and statistics to enable corpus learning and improve compression over time.

## Database Location

- **Windows**: `%PROGRAMDATA%\RDX\LCM\lcm.db`
- **Linux**: `$XDG_DATA_HOME/rdx/lcm.db` or `$HOME/.local/share/rdx/lcm.db`

## Tables

### `meta`

Metadata about the LCM database itself.

| Column | Type | Description |
|--------|------|-------------|
| `schema_version` | INTEGER | LCM schema version |
| `created_at` | INTEGER | Unix timestamp of creation |
| `last_updated_at` | INTEGER | Unix timestamp of last update |
| `host_id` | TEXT | Unique host identifier |
| `codec_version` | TEXT | RDX codec version |

### `file_index`

Index of all files ever compressed.

| Column | Type | Description |
|--------|------|-------------|
| `file_id` | INTEGER PRIMARY KEY | Unique file identifier |
| `content_hash` | TEXT | SHA-256 hash of file content |
| `path_hash` | TEXT | Hash of file path (privacy-preserving) |
| `size_bytes` | INTEGER | Original file size in bytes |
| `file_type_id` | INTEGER | Foreign key to `file_types` |
| `schema_id` | INTEGER | Foreign key to `schema_registry` |
| `first_seen_at` | INTEGER | Unix timestamp of first compression |
| `last_seen_at` | INTEGER | Unix timestamp of last compression |
| `bundle_id` | INTEGER | Foreign key to `bundles` (optional) |

**Indices**:
- `idx_file_content_hash` on `content_hash`
- `idx_file_path_hash` on `path_hash`
- `idx_file_type` on `file_type_id`

### `file_types`

Registered file types.

| Column | Type | Description |
|--------|------|-------------|
| `file_type_id` | INTEGER PRIMARY KEY | Unique file type identifier |
| `name` | TEXT UNIQUE | File type name (e.g., "pe32", "json") |
| `detector_signature` | TEXT | Signature used for detection |

### `bundles`

Groups of files compressed together (e.g., archive entries).

| Column | Type | Description |
|--------|------|-------------|
| `bundle_id` | INTEGER PRIMARY KEY | Unique bundle identifier |
| `bundle_hash` | TEXT | Hash of bundle contents |
| `label` | TEXT | Human-readable label |
| `created_at` | INTEGER | Unix timestamp of creation |

### `chunk_index`

Index of file chunks for similarity matching.

| Column | Type | Description |
|--------|------|-------------|
| `chunk_id` | INTEGER PRIMARY KEY | Unique chunk identifier |
| `file_id` | INTEGER | Foreign key to `file_index` |
| `offset_bytes` | INTEGER | Byte offset in original file |
| `length_bytes` | INTEGER | Chunk length in bytes |
| `chunk_hash` | TEXT | SHA-256 hash of chunk |
| `chunk_fingerprint` | INTEGER | Fast hash for similarity matching |
| `schema_id` | INTEGER | Foreign key to `schema_registry` |
| `token_profile_id` | INTEGER | Foreign key to `token_profiles` (optional) |
| `seen_count` | INTEGER | Number of times chunk seen |

**Indices**:
- `idx_chunk_file_id` on `file_id`
- `idx_chunk_hash` on `chunk_hash`
- `idx_chunk_fingerprint` on `chunk_fingerprint`

### `token_profiles`

Statistical token profiles for different file types.

| Column | Type | Description |
|--------|------|-------------|
| `token_profile_id` | INTEGER PRIMARY KEY | Unique profile identifier |
| `file_type_id` | INTEGER | Foreign key to `file_types` |
| `ngram_order` | INTEGER | N-gram order (e.g., 2 for bigrams) |
| `vocab_id` | INTEGER | Foreign key to `vocabularies` |
| `stats_blob` | TEXT | JSON or binary statistics |
| `created_at` | INTEGER | Unix timestamp of creation |
| `updated_at` | INTEGER | Unix timestamp of last update |
| `usage_count` | INTEGER | Number of times profile used |

### `vocabularies`

Vocabulary models for file types.

| Column | Type | Description |
|--------|------|-------------|
| `vocab_id` | INTEGER PRIMARY KEY | Unique vocabulary identifier |
| `file_type_id` | INTEGER | Foreign key to `file_types` |
| `version` | INTEGER | Vocabulary version |
| `vocab_blob` | BLOB | Serialized vocabulary data |

### `schema_registry`

All registered schemas.

| Column | Type | Description |
|--------|------|-------------|
| `schema_id` | INTEGER PRIMARY KEY | Unique schema identifier |
| `name` | TEXT | Schema name (e.g., "PE32", "JSON_GENERIC") |
| `version` | INTEGER | Schema version |
| `definition` | TEXT | JSON schema definition |
| `created_at` | INTEGER | Unix timestamp of creation |
| `updated_at` | INTEGER | Unix timestamp of last update |
| `usage_count` | INTEGER | Number of times schema used |

**Unique Constraint**: `(name, version)`

**Indices**:
- `idx_schema_usage` on `usage_count`

### `schema_stats`

Per-field statistics for schemas.

| Column | Type | Description |
|--------|------|-------------|
| `schema_id` | INTEGER | Foreign key to `schema_registry` |
| `field_name` | TEXT | Field name |
| `field_kind` | TEXT | Field kind (Integer, String, etc.) |
| `stats_blob` | TEXT | JSON or binary statistics |

**Primary Key**: `(schema_id, field_name)`

### `generator_index`

Index of generators for blob-like regions (future use).

| Column | Type | Description |
|--------|------|-------------|
| `generator_id` | INTEGER PRIMARY KEY | Unique generator identifier |
| `domain` | TEXT | Generator domain |
| `description_blob` | TEXT | Generator description |
| `usage_count` | INTEGER | Number of times generator used |
| `avg_param_bits` | REAL | Average parameter bits |
| `avg_residual_bits` | REAL | Average residual bits |

## Usage Patterns

### File Registration

When a file is compressed:
1. Compute content hash and path hash
2. Detect file type (or use existing)
3. Select schema (or use default)
4. Insert into `file_index`
5. Record chunks in `chunk_index`
6. Increment schema usage count

### Similarity Matching

To find similar chunks:
1. Compute chunk fingerprint
2. Query `chunk_index` by `chunk_fingerprint`
3. Order by `seen_count` DESC
4. Use top matches for compression hints

### Schema Statistics

To update schema statistics:
1. Parse file with schema
2. Collect field statistics
3. Update `schema_stats` table
4. Increment `schema_registry.usage_count`

## Maintenance

- **VACUUM**: Periodically run `VACUUM` to reclaim space
- **Optimize**: Run `PRAGMA optimize` for query planner hints
- **Backup**: LCM database can be backed up for corpus preservation


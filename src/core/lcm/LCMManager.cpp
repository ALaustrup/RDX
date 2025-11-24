#include "lcm/LCMManager.h"
#include "util/HashUtils.h"
#include "util/TimeUtils.h"
#include <sqlite3.h>
#include <stdexcept>
#include <algorithm>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <cstdlib>
#include <pwd.h>
#include <unistd.h>
#endif

namespace rdx::core {

std::unique_ptr<LCMManager> LCMManager::createDefault() {
    std::filesystem::path dbPath;
    
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, path))) {
        dbPath = std::filesystem::path(path) / "RDX" / "LCM" / "lcm.db";
    } else {
        dbPath = std::filesystem::path("C:\\ProgramData\\RDX\\LCM\\lcm.db");
    }
#else
    const char* xdgDataHome = std::getenv("XDG_DATA_HOME");
    if (xdgDataHome) {
        dbPath = std::filesystem::path(xdgDataHome) / "rdx" / "lcm.db";
    } else {
        const char* home = std::getenv("HOME");
        if (home) {
            dbPath = std::filesystem::path(home) / ".local" / "share" / "rdx" / "lcm.db";
        } else {
            dbPath = std::filesystem::path(".") / "lcm.db";
        }
    }
#endif
    
    // Create parent directories
    std::filesystem::create_directories(dbPath.parent_path());
    
    return std::make_unique<LCMManager>(dbPath);
}

LCMManager::LCMManager(const std::filesystem::path& dbPath)
    : db_(nullptr)
    , dbPath_(dbPath)
    , stmtRegisterFile_(nullptr)
    , stmtFindFileByHash_(nullptr)
    , stmtRecordChunk_(nullptr)
    , stmtFindChunksByFingerprint_(nullptr)
    , stmtGetOrCreateFileType_(nullptr)
    , stmtGetOrCreateSchema_(nullptr)
    , stmtIncrementSchemaUsage_(nullptr) {
    
    int rc = sqlite3_open(dbPath.string().c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("Failed to open LCM database: " + error);
    }
    
    initializeSchema();
    prepareStatements();
}

LCMManager::~LCMManager() {
    finalizeStatements();
    if (db_) {
        sqlite3_close(db_);
    }
}

LCMManager::LCMManager(LCMManager&& other) noexcept
    : db_(other.db_)
    , dbPath_(std::move(other.dbPath_))
    , stmtRegisterFile_(other.stmtRegisterFile_)
    , stmtFindFileByHash_(other.stmtFindFileByHash_)
    , stmtRecordChunk_(other.stmtRecordChunk_)
    , stmtFindChunksByFingerprint_(other.stmtFindChunksByFingerprint_)
    , stmtGetOrCreateFileType_(other.stmtGetOrCreateFileType_)
    , stmtGetOrCreateSchema_(other.stmtGetOrCreateSchema_)
    , stmtIncrementSchemaUsage_(other.stmtIncrementSchemaUsage_) {
    other.db_ = nullptr;
    other.stmtRegisterFile_ = nullptr;
    other.stmtFindFileByHash_ = nullptr;
    other.stmtRecordChunk_ = nullptr;
    other.stmtFindChunksByFingerprint_ = nullptr;
    other.stmtGetOrCreateFileType_ = nullptr;
    other.stmtGetOrCreateSchema_ = nullptr;
    other.stmtIncrementSchemaUsage_ = nullptr;
}

LCMManager& LCMManager::operator=(LCMManager&& other) noexcept {
    if (this != &other) {
        finalizeStatements();
        if (db_) {
            sqlite3_close(db_);
        }
        
        db_ = other.db_;
        dbPath_ = std::move(other.dbPath_);
        stmtRegisterFile_ = other.stmtRegisterFile_;
        stmtFindFileByHash_ = other.stmtFindFileByHash_;
        stmtRecordChunk_ = other.stmtRecordChunk_;
        stmtFindChunksByFingerprint_ = other.stmtFindChunksByFingerprint_;
        stmtGetOrCreateFileType_ = other.stmtGetOrCreateFileType_;
        stmtGetOrCreateSchema_ = other.stmtGetOrCreateSchema_;
        stmtIncrementSchemaUsage_ = other.stmtIncrementSchemaUsage_;
        
        other.db_ = nullptr;
        other.stmtRegisterFile_ = nullptr;
        other.stmtFindFileByHash_ = nullptr;
        other.stmtRecordChunk_ = nullptr;
        other.stmtFindChunksByFingerprint_ = nullptr;
        other.stmtGetOrCreateFileType_ = nullptr;
        other.stmtGetOrCreateSchema_ = nullptr;
        other.stmtIncrementSchemaUsage_ = nullptr;
    }
    return *this;
}

void LCMManager::initializeSchema() {
    createTables();
    createIndices();
}

void LCMManager::createTables() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS meta (
            schema_version INTEGER NOT NULL,
            created_at INTEGER NOT NULL,
            last_updated_at INTEGER NOT NULL,
            host_id TEXT,
            codec_version TEXT
        );
        
        CREATE TABLE IF NOT EXISTS file_index (
            file_id INTEGER PRIMARY KEY AUTOINCREMENT,
            content_hash TEXT NOT NULL,
            path_hash TEXT,
            size_bytes INTEGER NOT NULL,
            file_type_id INTEGER NOT NULL,
            schema_id INTEGER,
            first_seen_at INTEGER NOT NULL,
            last_seen_at INTEGER NOT NULL,
            bundle_id INTEGER,
            FOREIGN KEY (file_type_id) REFERENCES file_types(file_type_id),
            FOREIGN KEY (schema_id) REFERENCES schema_registry(schema_id),
            FOREIGN KEY (bundle_id) REFERENCES bundles(bundle_id)
        );
        
        CREATE TABLE IF NOT EXISTS file_types (
            file_type_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            detector_signature TEXT
        );
        
        CREATE TABLE IF NOT EXISTS bundles (
            bundle_id INTEGER PRIMARY KEY AUTOINCREMENT,
            bundle_hash TEXT NOT NULL,
            label TEXT,
            created_at INTEGER NOT NULL
        );
        
        CREATE TABLE IF NOT EXISTS chunk_index (
            chunk_id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_id INTEGER NOT NULL,
            offset_bytes INTEGER NOT NULL,
            length_bytes INTEGER NOT NULL,
            chunk_hash TEXT NOT NULL,
            chunk_fingerprint INTEGER NOT NULL,
            schema_id INTEGER,
            token_profile_id INTEGER,
            seen_count INTEGER DEFAULT 1,
            FOREIGN KEY (file_id) REFERENCES file_index(file_id),
            FOREIGN KEY (schema_id) REFERENCES schema_registry(schema_id),
            FOREIGN KEY (token_profile_id) REFERENCES token_profiles(token_profile_id)
        );
        
        CREATE TABLE IF NOT EXISTS token_profiles (
            token_profile_id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_type_id INTEGER NOT NULL,
            ngram_order INTEGER,
            vocab_id INTEGER,
            stats_blob TEXT,
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL,
            usage_count INTEGER DEFAULT 0,
            FOREIGN KEY (file_type_id) REFERENCES file_types(file_type_id)
        );
        
        CREATE TABLE IF NOT EXISTS vocabularies (
            vocab_id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_type_id INTEGER NOT NULL,
            version INTEGER,
            vocab_blob BLOB,
            FOREIGN KEY (file_type_id) REFERENCES file_types(file_type_id)
        );
        
        CREATE TABLE IF NOT EXISTS schema_registry (
            schema_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            version INTEGER NOT NULL,
            definition TEXT NOT NULL,
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL,
            usage_count INTEGER DEFAULT 0,
            UNIQUE(name, version)
        );
        
        CREATE TABLE IF NOT EXISTS schema_stats (
            schema_id INTEGER NOT NULL,
            field_name TEXT NOT NULL,
            field_kind TEXT NOT NULL,
            stats_blob TEXT,
            FOREIGN KEY (schema_id) REFERENCES schema_registry(schema_id),
            PRIMARY KEY (schema_id, field_name)
        );
        
        CREATE TABLE IF NOT EXISTS generator_index (
            generator_id INTEGER PRIMARY KEY AUTOINCREMENT,
            domain TEXT,
            description_blob TEXT,
            usage_count INTEGER DEFAULT 0,
            avg_param_bits REAL,
            avg_residual_bits REAL
        );
    )";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string error = errMsg ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error("Failed to create LCM tables: " + error);
    }
    
    // Initialize meta if empty
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db_, "SELECT COUNT(*) FROM meta", -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 0) {
            sqlite3_finalize(stmt);
            std::int64_t now = getCurrentTimestamp();
            std::ostringstream oss;
            oss << "INSERT INTO meta (schema_version, created_at, last_updated_at, codec_version) VALUES (1, " 
                << now << ", " << now << ", '1.0.0')";
            sqlite3_exec(db_, oss.str().c_str(), nullptr, nullptr, nullptr);
        } else {
            sqlite3_finalize(stmt);
        }
    }
}

void LCMManager::createIndices() {
    const char* sql = R"(
        CREATE INDEX IF NOT EXISTS idx_file_content_hash ON file_index(content_hash);
        CREATE INDEX IF NOT EXISTS idx_file_path_hash ON file_index(path_hash);
        CREATE INDEX IF NOT EXISTS idx_file_type ON file_index(file_type_id);
        CREATE INDEX IF NOT EXISTS idx_chunk_file_id ON chunk_index(file_id);
        CREATE INDEX IF NOT EXISTS idx_chunk_hash ON chunk_index(chunk_hash);
        CREATE INDEX IF NOT EXISTS idx_chunk_fingerprint ON chunk_index(chunk_fingerprint);
        CREATE INDEX IF NOT EXISTS idx_schema_usage ON schema_registry(usage_count);
    )";
    
    sqlite3_exec(db_, sql, nullptr, nullptr, nullptr);
}

void LCMManager::prepareStatements() {
    // In production, properly prepare and cache statements
    // For now, we'll use direct execution for simplicity
}

void LCMManager::finalizeStatements() {
    if (stmtRegisterFile_) sqlite3_finalize(stmtRegisterFile_);
    if (stmtFindFileByHash_) sqlite3_finalize(stmtFindFileByHash_);
    if (stmtRecordChunk_) sqlite3_finalize(stmtRecordChunk_);
    if (stmtFindChunksByFingerprint_) sqlite3_finalize(stmtFindChunksByFingerprint_);
    if (stmtGetOrCreateFileType_) sqlite3_finalize(stmtGetOrCreateFileType_);
    if (stmtGetOrCreateSchema_) sqlite3_finalize(stmtGetOrCreateSchema_);
    if (stmtIncrementSchemaUsage_) sqlite3_finalize(stmtIncrementSchemaUsage_);
}

int LCMManager::registerFile(const FileInfo& info) {
    std::ostringstream oss;
    oss << "INSERT INTO file_index (content_hash, path_hash, size_bytes, file_type_id, schema_id, "
        << "first_seen_at, last_seen_at, bundle_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, oss.str().c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare register file statement");
    }
    
    sqlite3_bind_text(stmt, 1, info.contentHash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, info.pathHash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 3, info.sizeBytes);
    sqlite3_bind_int(stmt, 4, info.fileTypeId);
    sqlite3_bind_int(stmt, 5, info.schemaId);
    sqlite3_bind_int64(stmt, 6, info.firstSeenAt);
    sqlite3_bind_int64(stmt, 7, info.lastSeenAt);
    if (info.bundleId.has_value()) {
        sqlite3_bind_int(stmt, 8, info.bundleId.value());
    } else {
        sqlite3_bind_null(stmt, 8);
    }
    
    rc = sqlite3_step(stmt);
    int fileId = -1;
    if (rc == SQLITE_DONE) {
        fileId = static_cast<int>(sqlite3_last_insert_rowid(db_));
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to register file");
    }
    
    return fileId;
}

std::optional<FileInfo> LCMManager::findFileByContentHash(const std::string& contentHash) const {
    const char* sql = "SELECT file_id, content_hash, path_hash, size_bytes, file_type_id, "
                      "schema_id, first_seen_at, last_seen_at, bundle_id FROM file_index WHERE content_hash = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return std::nullopt;
    }
    
    sqlite3_bind_text(stmt, 1, contentHash.c_str(), -1, SQLITE_STATIC);
    
    std::optional<FileInfo> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        FileInfo info;
        info.contentHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        info.pathHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        info.sizeBytes = sqlite3_column_int64(stmt, 3);
        info.fileTypeId = sqlite3_column_int(stmt, 4);
        info.schemaId = sqlite3_column_int(stmt, 5);
        info.firstSeenAt = sqlite3_column_int64(stmt, 6);
        info.lastSeenAt = sqlite3_column_int64(stmt, 7);
        if (sqlite3_column_type(stmt, 8) != SQLITE_NULL) {
            info.bundleId = sqlite3_column_int(stmt, 8);
        }
        result = info;
    }
    
    sqlite3_finalize(stmt);
    return result;
}

void LCMManager::updateFileLastSeen(int fileId, std::int64_t timestamp) {
    std::ostringstream oss;
    oss << "UPDATE file_index SET last_seen_at = ? WHERE file_id = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, oss.str().c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, timestamp);
        sqlite3_bind_int(stmt, 2, fileId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

void LCMManager::recordChunks(const std::vector<FileChunkInfo>& chunks) {
    const char* sql = "INSERT INTO chunk_index (file_id, offset_bytes, length_bytes, chunk_hash, "
                      "chunk_fingerprint, schema_id, token_profile_id, seen_count) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    
    for (const auto& chunk : chunks) {
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, chunk.fileId);
            sqlite3_bind_int64(stmt, 2, chunk.offsetBytes);
            sqlite3_bind_int64(stmt, 3, chunk.lengthBytes);
            sqlite3_bind_text(stmt, 4, chunk.chunkHash.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int64(stmt, 5, static_cast<std::int64_t>(chunk.chunkFingerprint));
            sqlite3_bind_int(stmt, 6, chunk.schemaId);
            if (chunk.tokenProfileId.has_value()) {
                sqlite3_bind_int(stmt, 7, chunk.tokenProfileId.value());
            } else {
                sqlite3_bind_null(stmt, 7);
            }
            sqlite3_bind_int64(stmt, 8, chunk.seenCount);
            
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }
}

std::vector<ChunkMatch> LCMManager::findSimilarChunks(std::uint64_t fingerprint, std::size_t limit) const {
    const char* sql = "SELECT chunk_id, file_id, chunk_fingerprint FROM chunk_index "
                      "WHERE chunk_fingerprint = ? ORDER BY seen_count DESC LIMIT ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return {};
    }
    
    sqlite3_bind_int64(stmt, 1, static_cast<std::int64_t>(fingerprint));
    sqlite3_bind_int64(stmt, 2, static_cast<std::int64_t>(limit));
    
    std::vector<ChunkMatch> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ChunkMatch match;
        match.chunkId = sqlite3_column_int(stmt, 0);
        match.fileId = sqlite3_column_int(stmt, 1);
        match.fingerprint = static_cast<std::uint64_t>(sqlite3_column_int64(stmt, 2));
        match.similarity = 1.0; // Exact match
        results.push_back(match);
    }
    
    sqlite3_finalize(stmt);
    return results;
}

std::vector<ChunkMatch> LCMManager::findSimilarChunksByHash(const std::string& chunkHash, std::size_t limit) const {
    const char* sql = "SELECT chunk_id, file_id, chunk_fingerprint FROM chunk_index "
                      "WHERE chunk_hash = ? ORDER BY seen_count DESC LIMIT ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return {};
    }
    
    sqlite3_bind_text(stmt, 1, chunkHash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, static_cast<std::int64_t>(limit));
    
    std::vector<ChunkMatch> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ChunkMatch match;
        match.chunkId = sqlite3_column_int(stmt, 0);
        match.fileId = sqlite3_column_int(stmt, 1);
        match.fingerprint = static_cast<std::uint64_t>(sqlite3_column_int64(stmt, 2));
        match.similarity = 1.0;
        results.push_back(match);
    }
    
    sqlite3_finalize(stmt);
    return results;
}

int LCMManager::getOrCreateFileTypeId(const std::string& name, const std::string& detectorSignature) {
    // Try to find existing
    const char* sql = "SELECT file_type_id FROM file_types WHERE name = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
            return id;
        }
        sqlite3_finalize(stmt);
    }
    
    // Create new
    sql = "INSERT INTO file_types (name, detector_signature) VALUES (?, ?)";
    rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, detectorSignature.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        int id = static_cast<int>(sqlite3_last_insert_rowid(db_));
        sqlite3_finalize(stmt);
        return id;
    }
    
    return -1;
}

std::string LCMManager::getFileTypeName(int fileTypeId) const {
    const char* sql = "SELECT name FROM file_types WHERE file_type_id = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, fileTypeId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            sqlite3_finalize(stmt);
            return name;
        }
        sqlite3_finalize(stmt);
    }
    return "";
}

int LCMManager::getOrCreateSchemaId(const std::string& name, int version, const std::string& definition) {
    // Try to find existing
    const char* sql = "SELECT schema_id FROM schema_registry WHERE name = ? AND version = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, version);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
            return id;
        }
        sqlite3_finalize(stmt);
    }
    
    // Create new
    std::int64_t now = getCurrentTimestamp();
    std::ostringstream oss;
    oss << "INSERT INTO schema_registry (name, version, definition, created_at, updated_at) "
        << "VALUES (?, ?, ?, " << now << ", " << now << ")";
    rc = sqlite3_prepare_v2(db_, oss.str().c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, version);
        sqlite3_bind_text(stmt, 3, definition.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        int id = static_cast<int>(sqlite3_last_insert_rowid(db_));
        sqlite3_finalize(stmt);
        return id;
    }
    
    return -1;
}

std::optional<std::string> LCMManager::loadSchemaDefinition(int schemaId) const {
    const char* sql = "SELECT definition FROM schema_registry WHERE schema_id = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, schemaId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string def = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            sqlite3_finalize(stmt);
            return def;
        }
        sqlite3_finalize(stmt);
    }
    return std::nullopt;
}

void LCMManager::incrementSchemaUsage(int schemaId) {
    const char* sql = "UPDATE schema_registry SET usage_count = usage_count + 1 WHERE schema_id = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, schemaId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

void LCMManager::updateSchemaStats(int schemaId, const std::vector<FieldStatUpdate>& updates) {
    for (const auto& update : updates) {
        const char* sql = "INSERT OR REPLACE INTO schema_stats (schema_id, field_name, field_kind, stats_blob) "
                          "VALUES (?, ?, ?, ?)";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, schemaId);
            sqlite3_bind_text(stmt, 2, update.fieldName.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, update.fieldKind.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, update.statsBlob.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }
}

std::vector<std::pair<int, int>> LCMManager::getTopSchemasByUsage(std::size_t limit) const {
    const char* sql = "SELECT schema_id, usage_count FROM schema_registry ORDER BY usage_count DESC LIMIT ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return {};
    }
    
    sqlite3_bind_int64(stmt, 1, static_cast<std::int64_t>(limit));
    
    std::vector<std::pair<int, int>> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.emplace_back(sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1));
    }
    
    sqlite3_finalize(stmt);
    return results;
}

int LCMManager::createBundle(const std::string& bundleHash, const std::string& label) {
    std::int64_t now = getCurrentTimestamp();
    std::ostringstream oss;
    oss << "INSERT INTO bundles (bundle_hash, label, created_at) VALUES (?, ?, " << now << ")";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, oss.str().c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, bundleHash.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, label.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        int id = static_cast<int>(sqlite3_last_insert_rowid(db_));
        sqlite3_finalize(stmt);
        return id;
    }
    return -1;
}

void LCMManager::associateFileWithBundle(int fileId, int bundleId) {
    const char* sql = "UPDATE file_index SET bundle_id = ? WHERE file_id = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, bundleId);
        sqlite3_bind_int(stmt, 2, fileId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

int LCMManager::getOrCreateTokenProfileId(int fileTypeId, int ngramOrder, int vocabId, const std::string& statsBlob) {
    // Simplified - in production, check for existing profile
    std::int64_t now = getCurrentTimestamp();
    std::ostringstream oss;
    oss << "INSERT INTO token_profiles (file_type_id, ngram_order, vocab_id, stats_blob, created_at, updated_at) "
        << "VALUES (?, ?, ?, ?, " << now << ", " << now << ")";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, oss.str().c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, fileTypeId);
        sqlite3_bind_int(stmt, 2, ngramOrder);
        sqlite3_bind_int(stmt, 3, vocabId);
        sqlite3_bind_text(stmt, 4, statsBlob.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        int id = static_cast<int>(sqlite3_last_insert_rowid(db_));
        sqlite3_finalize(stmt);
        return id;
    }
    return -1;
}

void LCMManager::updateTokenProfileUsage(int tokenProfileId) {
    const char* sql = "UPDATE token_profiles SET usage_count = usage_count + 1 WHERE token_profile_id = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, tokenProfileId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

std::int64_t LCMManager::getTotalFilesTracked() const {
    const char* sql = "SELECT COUNT(*) FROM file_index";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        std::int64_t count = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
        return count;
    }
    sqlite3_finalize(stmt);
    return 0;
}

std::int64_t LCMManager::getTotalCorpusSize() const {
    const char* sql = "SELECT SUM(size_bytes) FROM file_index";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        std::int64_t size = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
        return size;
    }
    sqlite3_finalize(stmt);
    return 0;
}

std::vector<std::pair<int, std::int64_t>> LCMManager::getTopFileTypes(std::size_t limit) const {
    const char* sql = "SELECT file_type_id, COUNT(*) as cnt FROM file_index GROUP BY file_type_id "
                      "ORDER BY cnt DESC LIMIT ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return {};
    }
    
    sqlite3_bind_int64(stmt, 1, static_cast<std::int64_t>(limit));
    
    std::vector<std::pair<int, std::int64_t>> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.emplace_back(sqlite3_column_int(stmt, 0), sqlite3_column_int64(stmt, 1));
    }
    
    sqlite3_finalize(stmt);
    return results;
}

void LCMManager::vacuum() {
    sqlite3_exec(db_, "VACUUM", nullptr, nullptr, nullptr);
}

void LCMManager::optimize() {
    sqlite3_exec(db_, "PRAGMA optimize", nullptr, nullptr, nullptr);
}

} // namespace rdx::core


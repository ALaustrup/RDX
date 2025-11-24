#ifndef RDX_LCMMANAGER_H
#define RDX_LCMMANAGER_H

#include <memory>
#include <filesystem>
#include <vector>
#include <string>
#include <optional>
#include <cstdint>
#include <span>
#include <cstddef>

struct sqlite3;
struct sqlite3_stmt;

namespace rdx::core {

struct FileInfo {
    std::string contentHash;
    std::string pathHash;
    std::int64_t sizeBytes;
    int fileTypeId;
    int schemaId;
    std::int64_t firstSeenAt;
    std::int64_t lastSeenAt;
    std::optional<int> bundleId;
};

struct FileChunkInfo {
    int fileId;
    std::int64_t offsetBytes;
    std::int64_t lengthBytes;
    std::string chunkHash;
    std::uint64_t chunkFingerprint;
    int schemaId;
    std::optional<int> tokenProfileId;
    std::int64_t seenCount;
};

struct ChunkMatch {
    int chunkId;
    int fileId;
    std::uint64_t fingerprint;
    double similarity;
};

struct FieldStatUpdate {
    std::string fieldName;
    std::string fieldKind;
    std::string statsBlob; // JSON or binary stats
};

class LCMManager {
public:
    static std::unique_ptr<LCMManager> createDefault();
    explicit LCMManager(const std::filesystem::path& dbPath);
    ~LCMManager();
    
    // Disable copy
    LCMManager(const LCMManager&) = delete;
    LCMManager& operator=(const LCMManager&) = delete;
    
    // Enable move
    LCMManager(LCMManager&&) noexcept;
    LCMManager& operator=(LCMManager&&) noexcept;
    
    void initializeSchema();
    
    // File operations
    int registerFile(const FileInfo& info);
    std::optional<FileInfo> findFileByContentHash(const std::string& contentHash) const;
    void updateFileLastSeen(int fileId, std::int64_t timestamp);
    
    // Chunk operations
    void recordChunks(const std::vector<FileChunkInfo>& chunks);
    std::vector<ChunkMatch> findSimilarChunks(std::uint64_t fingerprint, std::size_t limit) const;
    std::vector<ChunkMatch> findSimilarChunksByHash(const std::string& chunkHash, std::size_t limit) const;
    
    // File type operations
    int getOrCreateFileTypeId(const std::string& name, const std::string& detectorSignature);
    std::string getFileTypeName(int fileTypeId) const;
    
    // Schema operations
    int getOrCreateSchemaId(const std::string& name, int version, const std::string& definition);
    std::optional<std::string> loadSchemaDefinition(int schemaId) const;
    void incrementSchemaUsage(int schemaId);
    void updateSchemaStats(int schemaId, const std::vector<FieldStatUpdate>& updates);
    std::vector<std::pair<int, int>> getTopSchemasByUsage(std::size_t limit) const; // returns (schemaId, usageCount)
    
    // Bundle operations
    int createBundle(const std::string& bundleHash, const std::string& label);
    void associateFileWithBundle(int fileId, int bundleId);
    
    // Token profile operations
    int getOrCreateTokenProfileId(int fileTypeId, int ngramOrder, int vocabId, const std::string& statsBlob);
    void updateTokenProfileUsage(int tokenProfileId);
    
    // Statistics
    std::int64_t getTotalFilesTracked() const;
    std::int64_t getTotalCorpusSize() const;
    std::vector<std::pair<int, std::int64_t>> getTopFileTypes(std::size_t limit) const; // returns (fileTypeId, count)
    
    // Database maintenance
    void vacuum();
    void optimize();

private:
    sqlite3* db_;
    std::filesystem::path dbPath_;
    
    void createTables();
    void createIndices();
    void prepareStatements();
    
    // Prepared statements (simplified - in production, use proper prepared statement management)
    mutable sqlite3_stmt* stmtRegisterFile_;
    mutable sqlite3_stmt* stmtFindFileByHash_;
    mutable sqlite3_stmt* stmtRecordChunk_;
    mutable sqlite3_stmt* stmtFindChunksByFingerprint_;
    mutable sqlite3_stmt* stmtGetOrCreateFileType_;
    mutable sqlite3_stmt* stmtGetOrCreateSchema_;
    mutable sqlite3_stmt* stmtIncrementSchemaUsage_;
    
    void finalizeStatements();
};

} // namespace rdx::core

#endif // RDX_LCMMANAGER_H


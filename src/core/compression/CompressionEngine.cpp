#include "compression/CompressionEngine.h"
#include "detectors/FileTypeDetector.h"
#include "schemas/parsers/UnstructuredBinaryParser.h"
#include "schemas/parsers/JSONParser.h"
#include "schemas/parsers/PE32Parser.h"
#include "schemas/parsers/LogParser.h"
#include "schemas/parsers/CSVParser.h"
#include "schemas/parsers/KVConfigParser.h"
#include "schemas/parsers/ChunkedBinaryParser.h"
#include "util/HashUtils.h"
#include "util/TimeUtils.h"
#include <zstd.h>
#include <fstream>
#include <algorithm>

namespace rdx::core {

CompressionEngine::CompressionEngine(LCMManager& lcm, SchemaRegistry& schemaRegistry)
    : lcm_(lcm)
    , schemaRegistry_(schemaRegistry) {
    initializeParsers();
}

void CompressionEngine::initializeParsers() {
    // Create parsers for each schema type
    auto& pe32Schema = schemaRegistry_.getSchemaById(SchemaRegistry::SCHEMA_PE32_ID);
    auto& jsonSchema = schemaRegistry_.getSchemaById(SchemaRegistry::SCHEMA_JSON_GENERIC_ID);
    auto& logSchema = schemaRegistry_.getSchemaById(SchemaRegistry::SCHEMA_LOG_LINE_ID);
    auto& csvSchema = schemaRegistry_.getSchemaById(SchemaRegistry::SCHEMA_CSV_SIMPLE_ID);
    auto& kvSchema = schemaRegistry_.getSchemaById(SchemaRegistry::SCHEMA_KV_CONFIG_ID);
    auto& chunkedSchema = schemaRegistry_.getSchemaById(SchemaRegistry::SCHEMA_CHUNKED_BINARY_ID);
    auto& unstructuredSchema = schemaRegistry_.getSchemaById(SchemaRegistry::SCHEMA_UNSTRUCTURED_BINARY_ID);
    
    parsers_.push_back(std::make_unique<PE32Parser>(pe32Schema));
    parsers_.push_back(std::make_unique<JSONParser>(jsonSchema));
    parsers_.push_back(std::make_unique<LogParser>(logSchema));
    parsers_.push_back(std::make_unique<CSVParser>(csvSchema));
    parsers_.push_back(std::make_unique<KVConfigParser>(kvSchema));
    parsers_.push_back(std::make_unique<ChunkedBinaryParser>(chunkedSchema));
    parsers_.push_back(std::make_unique<UnstructuredBinaryParser>(unstructuredSchema));
}

ISchemaParser* CompressionEngine::findParser(const DetectedFileType& fileType, std::span<const std::byte> prefix) {
    for (auto& parser : parsers_) {
        if (parser->canParse(fileType, prefix)) {
            return parser.get();
        }
    }
    return nullptr;
}

void CompressionEngine::compressWithZstd(std::span<const std::byte> data, ByteBuffer& out) {
    std::size_t maxSize = ZSTD_compressBound(data.size());
    out.resize(maxSize);
    
    std::size_t compressedSize = ZSTD_compress(
        out.mutableDataPtr(),
        maxSize,
        data.data(),
        data.size(),
        3  // compression level
    );
    
    if (ZSTD_isError(compressedSize)) {
        throw std::runtime_error("ZSTD compression failed");
    }
    
    out.resize(compressedSize);
}

CompressionResult CompressionEngine::compressFile(const std::filesystem::path& inputPath,
                                                   ByteBuffer& outStructStream,
                                                   ByteBuffer& outResidualStream) {
    CompressionResult result;
    
    // Read file
    std::ifstream file(inputPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + inputPath.string());
    }
    
    std::size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<std::byte> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    file.close();
    
    result.originalSize = static_cast<std::int64_t>(fileSize);
    
    // Detect file type
    FileTypeDetector detector;
    std::span<const std::byte> prefix(fileData.data(), std::min(fileData.size(), std::size_t(1024)));
    DetectedFileType fileType = detector.detect(inputPath, prefix);
    
    // Register file type in LCM
    int fileTypeId = lcm_.getOrCreateFileTypeId(fileType.name, fileType.detectorSignature);
    result.fileTypeId = fileTypeId;
    
    // Find parser
    ISchemaParser* parser = findParser(fileType, prefix);
    if (!parser) {
        parser = parsers_.back().get();  // Use unstructured parser as fallback
    }
    
    // Parse file
    ParsedRepresentation parsed = parser->parse(inputPath);
    const SchemaDefinition& schema = parser->getSchema();
    
    // Get or create schema ID
    int schemaId = schemaRegistry_.getSchemaId(schema.name, schema.version);
    if (schemaId == -1) {
        schemaId = schemaRegistry_.registerSchema(schema);
    }
    result.schemaId = schemaId;
    
    // Increment schema usage
    lcm_.incrementSchemaUsage(schemaId);
    
    // For now, serialize parsed representation as JSON-like structure (simplified)
    // In production, use proper encoding with arithmetic coding or similar
    std::string structData = "{\"schema\":" + std::to_string(schemaId) + "}";
    std::vector<std::byte> structBytes(structData.begin(), structData.end());
    
    // Compress structural data
    if (!structBytes.empty()) {
        compressWithZstd(std::span<const std::byte>(structBytes.data(), structBytes.size()), outStructStream);
    }
    
    // For residual, compress the original file data (simplified - in production, 
    // extract only the parts not captured by structure)
    if (!fileData.empty()) {
        compressWithZstd(std::span<const std::byte>(fileData.data(), fileData.size()), outResidualStream);
    }
    
    result.compressedStructSize = static_cast<std::int64_t>(outStructStream.size());
    result.compressedResidualSize = static_cast<std::int64_t>(outResidualStream.size());
    
    if (result.originalSize > 0) {
        result.compressionRatio = static_cast<double>(result.compressedStructSize + result.compressedResidualSize) 
                                  / static_cast<double>(result.originalSize);
    } else {
        result.compressionRatio = 1.0;
    }
    
    // Register file in LCM
    FileInfo fileInfo;
    fileInfo.contentHash = computeContentHash(std::span<const std::byte>(fileData.data(), fileData.size()));
    fileInfo.pathHash = computePathHash(inputPath.string());
    fileInfo.sizeBytes = result.originalSize;
    fileInfo.fileTypeId = fileTypeId;
    fileInfo.schemaId = schemaId;
    fileInfo.firstSeenAt = getCurrentTimestamp();
    fileInfo.lastSeenAt = fileInfo.firstSeenAt;
    
    int fileId = lcm_.registerFile(fileInfo);
    
    // Record chunks (simplified - in production, chunk intelligently)
    std::vector<FileChunkInfo> chunks;
    constexpr std::size_t chunkSize = 64 * 1024;  // 64KB chunks
    for (std::size_t offset = 0; offset < fileData.size(); offset += chunkSize) {
        std::size_t chunkLen = std::min(chunkSize, fileData.size() - offset);
        std::span<const std::byte> chunk(fileData.data() + offset, chunkLen);
        
        FileChunkInfo chunkInfo;
        chunkInfo.fileId = fileId;
        chunkInfo.offsetBytes = static_cast<std::int64_t>(offset);
        chunkInfo.lengthBytes = static_cast<std::int64_t>(chunkLen);
        chunkInfo.chunkHash = computeSHA256(chunk);
        chunkInfo.chunkFingerprint = computeChunkFingerprint(chunk);
        chunkInfo.schemaId = schemaId;
        chunkInfo.seenCount = 1;
        
        chunks.push_back(chunkInfo);
    }
    
    if (!chunks.empty()) {
        lcm_.recordChunks(chunks);
    }
    
    return result;
}

} // namespace rdx::core


#ifndef RDX_COMPRESSIONENGINE_H
#define RDX_COMPRESSIONENGINE_H

#include "lcm/LCMManager.h"
#include "schemas/SchemaRegistry.h"
#include "schemas/parsers/ISchemaParser.h"
#include "util/ByteBuffer.h"
#include <filesystem>
#include <memory>
#include <vector>

namespace rdx::core {

struct CompressionResult {
    std::int64_t originalSize;
    std::int64_t compressedStructSize;
    std::int64_t compressedResidualSize;
    int schemaId;
    int fileTypeId;
    double compressionRatio;
};

class CompressionEngine {
public:
    CompressionEngine(LCMManager& lcm, SchemaRegistry& schemaRegistry);
    
    CompressionResult compressFile(const std::filesystem::path& inputPath,
                                   ByteBuffer& outStructStream,
                                   ByteBuffer& outResidualStream);

private:
    LCMManager& lcm_;
    SchemaRegistry& schemaRegistry_;
    std::vector<std::unique_ptr<ISchemaParser>> parsers_;
    
    void initializeParsers();
    ISchemaParser* findParser(const DetectedFileType& fileType, std::span<const std::byte> prefix);
    void compressWithZstd(std::span<const std::byte> data, ByteBuffer& out);
};

} // namespace rdx::core

#endif // RDX_COMPRESSIONENGINE_H


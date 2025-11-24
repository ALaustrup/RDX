#ifndef RDX_DECOMPRESSIONENGINE_H
#define RDX_DECOMPRESSIONENGINE_H

#include "lcm/LCMManager.h"
#include "schemas/SchemaRegistry.h"
#include "container/RDXWriter.h"
#include "util/ByteBuffer.h"
#include <filesystem>

namespace rdx::core {

class DecompressionEngine {
public:
    DecompressionEngine(LCMManager& lcm, SchemaRegistry& schemaRegistry);
    
    void decompressToFile(const RDXEntry& entry,
                          const ByteBuffer& structStream,
                          const ByteBuffer& residualStream,
                          const std::filesystem::path& outputPath);

private:
    LCMManager& lcm_;
    SchemaRegistry& schemaRegistry_;
    
    void decompressWithZstd(std::span<const std::byte> compressed, 
                            std::size_t originalSize,
                            ByteBuffer& out);
};

} // namespace rdx::core

#endif // RDX_DECOMPRESSIONENGINE_H


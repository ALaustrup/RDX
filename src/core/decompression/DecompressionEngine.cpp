#include "decompression/DecompressionEngine.h"
#include <zstd.h>
#include <fstream>

namespace rdx::core {

DecompressionEngine::DecompressionEngine(LCMManager& lcm, SchemaRegistry& schemaRegistry)
    : lcm_(lcm)
    , schemaRegistry_(schemaRegistry) {
}

void DecompressionEngine::decompressWithZstd(std::span<const std::byte> compressed,
                                              std::size_t originalSize,
                                              ByteBuffer& out) {
    out.resize(originalSize);
    
    std::size_t decompressedSize = ZSTD_decompress(
        out.mutableDataPtr(),
        originalSize,
        compressed.data(),
        compressed.size()
    );
    
    if (ZSTD_isError(decompressedSize)) {
        throw std::runtime_error("ZSTD decompression failed: " + std::string(ZSTD_getErrorName(decompressedSize)));
    }
    
    out.resize(decompressedSize);
}

void DecompressionEngine::decompressToFile(const RDXEntry& entry,
                                            const ByteBuffer& structStream,
                                            const ByteBuffer& residualStream,
                                            const std::filesystem::path& outputPath) {
    // For now, simplified decompression: just decompress residual stream
    // In production, reconstruct from structural stream using schema and constraints
    
    ByteBuffer decompressed;
    
    // Decompress residual (which contains the full file in simplified version)
    decompressWithZstd(residualStream.data(), static_cast<std::size_t>(entry.originalSize), decompressed);
    
    // Write to file
    std::ofstream file(outputPath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open output file: " + outputPath.string());
    }
    
    file.write(reinterpret_cast<const char*>(decompressed.dataPtr()), decompressed.size());
    file.close();
}

} // namespace rdx::core


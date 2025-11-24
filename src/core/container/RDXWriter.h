#ifndef RDX_RDXWRITER_H
#define RDX_RDXWRITER_H

#include "util/ByteBuffer.h"
#include "compression/CompressionEngine.h"
#include <filesystem>
#include <vector>
#include <fstream>
#include <cstdint>

namespace rdx::core {

struct RDXEntry {
    std::string fileName;
    std::int64_t originalSize;
    std::int64_t compressedStructSize;
    std::int64_t compressedResidualSize;
    int schemaId;
    int fileTypeId;
    std::int64_t offset;
    std::int64_t blockSize;
};

class RDXWriter {
public:
    explicit RDXWriter(const std::filesystem::path& outputPath);
    ~RDXWriter();
    
    void addFile(const std::filesystem::path& inputPath,
                 CompressionEngine& engine,
                 const std::string& archivePath = "");
    
    void finalize();
    
    const std::vector<RDXEntry>& getEntries() const { return entries_; }

private:
    std::filesystem::path outputPath_;
    std::ofstream file_;
    std::vector<RDXEntry> entries_;
    std::int64_t currentOffset_;
    
    static constexpr std::uint32_t RDX_MAGIC = 0x52445801;  // "RDX" + version
    static constexpr std::uint16_t RDX_VERSION = 1;
    
    void writeHeader();
    void writeIndex();
    std::int64_t writeBlock(const ByteBuffer& structStream, const ByteBuffer& residualStream,
                           int schemaId, int fileTypeId, std::int64_t originalSize);
};

} // namespace rdx::core

#endif // RDX_RDXWRITER_H


#ifndef RDX_RDXREADER_H
#define RDX_RDXREADER_H

#include "container/RDXWriter.h"
#include <filesystem>
#include <vector>
#include <fstream>

namespace rdx::core {
    class DecompressionEngine;
}

namespace rdx::core {

class RDXReader {
public:
    explicit RDXReader(const std::filesystem::path& archivePath);
    ~RDXReader();
    
    void listEntries(std::vector<RDXEntry>& outEntries) const;
    
    void extractEntry(const RDXEntry& entry,
                      const std::filesystem::path& outputPath,
                      DecompressionEngine& engine);
    
    void readBlock(const RDXEntry& entry,
                   ByteBuffer& outStructStream,
                   ByteBuffer& outResidualStream);

private:
    std::filesystem::path archivePath_;
    std::ifstream file_;
    std::vector<RDXEntry> entries_;
    std::int64_t indexOffset_;
    
    void readHeader();
    void readIndex();
};

} // namespace rdx::core

#endif // RDX_RDXREADER_H


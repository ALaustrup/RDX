#include "container/RDXWriter.h"
#include "compression/CompressionEngine.h"
#include <cstring>

namespace rdx::core {

RDXWriter::RDXWriter(const std::filesystem::path& outputPath)
    : outputPath_(outputPath)
    , currentOffset_(0) {
    file_.open(outputPath, std::ios::binary | std::ios::out);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open RDX file for writing: " + outputPath.string());
    }
    
    writeHeader();
}

RDXWriter::~RDXWriter() {
    if (file_.is_open()) {
        finalize();
    }
}

void RDXWriter::writeHeader() {
    // Write magic number
    std::uint32_t magic = RDX_MAGIC;
    file_.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    
    // Write version
    std::uint16_t version = RDX_VERSION;
    file_.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    // Write flags (reserved for future use)
    std::uint16_t flags = 0;
    file_.write(reinterpret_cast<const char*>(&flags), sizeof(flags));
    
    // Reserve space for index offset (will be written in finalize)
    std::int64_t indexOffsetPlaceholder = 0;
    file_.write(reinterpret_cast<const char*>(&indexOffsetPlaceholder), sizeof(indexOffsetPlaceholder));
    
    currentOffset_ = sizeof(magic) + sizeof(version) + sizeof(flags) + sizeof(indexOffsetPlaceholder);
}

void RDXWriter::addFile(const std::filesystem::path& inputPath,
                        CompressionEngine& engine,
                        const std::string& archivePath) {
    ByteBuffer structStream;
    ByteBuffer residualStream;
    
    auto result = engine.compressFile(inputPath, structStream, residualStream);
    
    std::int64_t blockOffset = writeBlock(structStream, residualStream, result.schemaId, result.fileTypeId, result.originalSize);
    
    RDXEntry entry;
    entry.fileName = archivePath.empty() ? inputPath.filename().string() : archivePath;
    entry.originalSize = result.originalSize;
    entry.compressedStructSize = static_cast<std::int64_t>(structStream.size());
    entry.compressedResidualSize = static_cast<std::int64_t>(residualStream.size());
    entry.schemaId = result.schemaId;
    entry.fileTypeId = result.fileTypeId;
    entry.offset = blockOffset;
    entry.blockSize = sizeof(std::uint32_t) + sizeof(std::uint32_t) + 
                      sizeof(std::int32_t) + sizeof(std::int32_t) +
                      sizeof(std::int64_t) + sizeof(std::int64_t) + sizeof(std::int64_t) +
                      sizeof(std::uint16_t) +
                      static_cast<std::int64_t>(structStream.size() + residualStream.size());
    
    entries_.push_back(entry);
}

std::int64_t RDXWriter::writeBlock(const ByteBuffer& structStream, const ByteBuffer& residualStream,
                                   int schemaId, int fileTypeId, std::int64_t originalSize) {
    std::int64_t blockOffset = currentOffset_;
    
    // Block magic (simplified - in production, use proper block markers)
    std::uint32_t blockMagic = 0x424C4B01;  // "BLK" + version
    file_.write(reinterpret_cast<const char*>(&blockMagic), sizeof(blockMagic));
    
    // Block header size
    std::uint32_t headerSize = sizeof(std::uint32_t) + sizeof(std::uint32_t) +
                               sizeof(std::int32_t) + sizeof(std::int32_t) +
                               sizeof(std::int64_t) + sizeof(std::int64_t) + sizeof(std::int64_t) +
                               sizeof(std::uint16_t);
    file_.write(reinterpret_cast<const char*>(&headerSize), sizeof(headerSize));
    
    // Schema ID, File Type ID
    file_.write(reinterpret_cast<const char*>(&schemaId), sizeof(schemaId));
    file_.write(reinterpret_cast<const char*>(&fileTypeId), sizeof(fileTypeId));
    
    // Sizes
    std::int64_t structSize = static_cast<std::int64_t>(structStream.size());
    std::int64_t residualSize = static_cast<std::int64_t>(residualStream.size());
    file_.write(reinterpret_cast<const char*>(&originalSize), sizeof(originalSize));
    file_.write(reinterpret_cast<const char*>(&structSize), sizeof(structSize));
    file_.write(reinterpret_cast<const char*>(&residualSize), sizeof(residualSize));
    
    // Flags
    std::uint16_t flags = 0;
    file_.write(reinterpret_cast<const char*>(&flags), sizeof(flags));
    
    // Write streams
    if (structStream.size() > 0) {
        file_.write(reinterpret_cast<const char*>(structStream.dataPtr()), structStream.size());
    }
    if (residualStream.size() > 0) {
        file_.write(reinterpret_cast<const char*>(residualStream.dataPtr()), residualStream.size());
    }
    
    currentOffset_ = file_.tellp();
    return blockOffset;
}

void RDXWriter::writeIndex() {
    std::int64_t indexOffset = currentOffset_;
    
    // Write number of entries
    std::uint32_t entryCount = static_cast<std::uint32_t>(entries_.size());
    file_.write(reinterpret_cast<const char*>(&entryCount), sizeof(entryCount));
    
    // Write each entry
    for (const auto& entry : entries_) {
        // File name length and name
        std::uint32_t nameLen = static_cast<std::uint32_t>(entry.fileName.length());
        file_.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        file_.write(entry.fileName.c_str(), nameLen);
        
        // Entry metadata
        file_.write(reinterpret_cast<const char*>(&entry.originalSize), sizeof(entry.originalSize));
        file_.write(reinterpret_cast<const char*>(&entry.compressedStructSize), sizeof(entry.compressedStructSize));
        file_.write(reinterpret_cast<const char*>(&entry.compressedResidualSize), sizeof(entry.compressedResidualSize));
        file_.write(reinterpret_cast<const char*>(&entry.schemaId), sizeof(entry.schemaId));
        file_.write(reinterpret_cast<const char*>(&entry.fileTypeId), sizeof(entry.fileTypeId));
        file_.write(reinterpret_cast<const char*>(&entry.offset), sizeof(entry.offset));
        file_.write(reinterpret_cast<const char*>(&entry.blockSize), sizeof(entry.blockSize));
    }
    
    // Update index offset in header
    file_.seekp(sizeof(std::uint32_t) + sizeof(std::uint16_t) + sizeof(std::uint16_t));
    file_.write(reinterpret_cast<const char*>(&indexOffset), sizeof(indexOffset));
}

void RDXWriter::finalize() {
    if (!file_.is_open()) {
        return;
    }
    
    writeIndex();
    file_.close();
}

} // namespace rdx::core


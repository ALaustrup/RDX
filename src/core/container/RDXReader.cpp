#include "container/RDXReader.h"
#include "decompression/DecompressionEngine.h"
#include <cstring>

namespace rdx::core {

RDXReader::RDXReader(const std::filesystem::path& archivePath)
    : archivePath_(archivePath)
    , indexOffset_(0) {
    file_.open(archivePath, std::ios::binary | std::ios::in);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open RDX file for reading: " + archivePath.string());
    }
    
    readHeader();
    readIndex();
}

RDXReader::~RDXReader() {
    if (file_.is_open()) {
        file_.close();
    }
}

void RDXReader::readHeader() {
    // Read magic
    std::uint32_t magic;
    file_.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    
    if (magic != RDXWriter::getMagic()) {
        throw std::runtime_error("Invalid RDX file magic number");
    }
    
    // Read version
    std::uint16_t version;
    file_.read(reinterpret_cast<char*>(&version), sizeof(version));
    
    // Read flags
    std::uint16_t flags;
    file_.read(reinterpret_cast<char*>(&flags), sizeof(flags));
    
    // Read index offset
    file_.read(reinterpret_cast<char*>(&indexOffset_), sizeof(indexOffset_));
}

void RDXReader::readIndex() {
    file_.seekg(indexOffset_);
    
    std::uint32_t entryCount;
    file_.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount));
    
    entries_.reserve(entryCount);
    
    for (std::uint32_t i = 0; i < entryCount; ++i) {
        RDXEntry entry;
        
        // Read file name
        std::uint32_t nameLen;
        file_.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        entry.fileName.resize(nameLen);
        file_.read(entry.fileName.data(), nameLen);
        
        // Read entry metadata
        file_.read(reinterpret_cast<char*>(&entry.originalSize), sizeof(entry.originalSize));
        file_.read(reinterpret_cast<char*>(&entry.compressedStructSize), sizeof(entry.compressedStructSize));
        file_.read(reinterpret_cast<char*>(&entry.compressedResidualSize), sizeof(entry.compressedResidualSize));
        file_.read(reinterpret_cast<char*>(&entry.schemaId), sizeof(entry.schemaId));
        file_.read(reinterpret_cast<char*>(&entry.fileTypeId), sizeof(entry.fileTypeId));
        file_.read(reinterpret_cast<char*>(&entry.offset), sizeof(entry.offset));
        file_.read(reinterpret_cast<char*>(&entry.blockSize), sizeof(entry.blockSize));
        
        entries_.push_back(entry);
    }
}

void RDXReader::listEntries(std::vector<RDXEntry>& outEntries) const {
    outEntries = entries_;
}

void RDXReader::readBlock(const RDXEntry& entry,
                          ByteBuffer& outStructStream,
                          ByteBuffer& outResidualStream) {
    file_.seekg(entry.offset);
    
    // Read block magic
    std::uint32_t blockMagic;
    file_.read(reinterpret_cast<char*>(&blockMagic), sizeof(blockMagic));
    
    // Read header size
    std::uint32_t headerSize;
    file_.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
    
    // Skip header fields we don't need for reading
    file_.seekg(entry.offset + sizeof(blockMagic) + sizeof(headerSize) + headerSize);
    
    // Read struct stream
    outStructStream.resize(static_cast<std::size_t>(entry.compressedStructSize));
    file_.read(reinterpret_cast<char*>(outStructStream.mutableDataPtr()), entry.compressedStructSize);
    
    // Read residual stream
    outResidualStream.resize(static_cast<std::size_t>(entry.compressedResidualSize));
    file_.read(reinterpret_cast<char*>(outResidualStream.mutableDataPtr()), entry.compressedResidualSize);
}

void RDXReader::extractEntry(const RDXEntry& entry,
                             const std::filesystem::path& outputPath,
                             DecompressionEngine& engine) {
    ByteBuffer structStream;
    ByteBuffer residualStream;
    
    readBlock(entry, structStream, residualStream);
    
    engine.decompressToFile(entry, structStream, residualStream, outputPath);
}

} // namespace rdx::core


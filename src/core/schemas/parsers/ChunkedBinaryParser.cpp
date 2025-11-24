#include "schemas/parsers/ChunkedBinaryParser.h"
#include <fstream>
#include <cstring>

namespace rdx::core {

ChunkedBinaryParser::ChunkedBinaryParser(const SchemaDefinition& schema)
    : schema_(schema) {
}

bool ChunkedBinaryParser::canParse(const DetectedFileType& fileType,
                                    std::span<const std::byte> prefix) const {
    return fileType.name == "chunked_binary";
}

std::uint32_t ChunkedBinaryParser::readU32(std::span<const std::byte> data, std::size_t offset) const {
    if (offset + 4 > data.size()) {
        return 0;
    }
    std::uint32_t value;
    std::memcpy(&value, data.data() + offset, 4);
    return value;
}

ParsedRepresentation ChunkedBinaryParser::parse(const std::filesystem::path& path) {
    ParsedRepresentation result;
    result.schema = &schema_;
    
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    
    std::size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<std::byte> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    // Parse TLV chunks
    std::size_t offset = 0;
    while (offset + 8 <= data.size()) {
        std::uint32_t typeId = readU32(data, offset);
        std::uint32_t length = readU32(data, offset + 4);
        
        if (offset + 8 + length > data.size()) {
            break;  // Invalid chunk
        }
        
        ParsedField chunk;
        chunk.name = "chunk";
        chunk.kind = FieldKind::Record;
        
        ParsedField typeField;
        typeField.name = "type_id";
        typeField.kind = FieldKind::Integer;
        typeField.value = static_cast<std::int64_t>(typeId);
        
        ParsedField lengthField;
        lengthField.name = "length";
        lengthField.kind = FieldKind::Integer;
        lengthField.value = static_cast<std::int64_t>(length);
        
        ParsedField payload;
        payload.name = "payload";
        payload.kind = FieldKind::Bytes;
        std::vector<std::byte> payloadData(data.begin() + offset + 8, 
                                           data.begin() + offset + 8 + length);
        payload.value = payloadData;
        
        chunk.value = std::vector<ParsedField>{typeField, lengthField, payload};
        result.fields.push_back(chunk);
        
        offset += 8 + length;
    }
    
    return result;
}

} // namespace rdx::core


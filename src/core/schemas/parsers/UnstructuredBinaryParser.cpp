#include "schemas/parsers/UnstructuredBinaryParser.h"
#include <fstream>
#include <filesystem>

namespace rdx::core {

UnstructuredBinaryParser::UnstructuredBinaryParser(const SchemaDefinition& schema)
    : schema_(schema) {
}

bool UnstructuredBinaryParser::canParse(const DetectedFileType& fileType,
                                         std::span<const std::byte> prefix) const {
    // Unstructured parser can always parse (it's the fallback)
    return true;
}

ParsedRepresentation UnstructuredBinaryParser::parse(const std::filesystem::path& path) {
    ParsedRepresentation result;
    result.schema = &schema_;
    
    // Read entire file as bytes
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    
    std::size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<std::byte> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    // Create a single blob field
    ParsedField blobField;
    blobField.name = "blob";
    blobField.kind = FieldKind::Bytes;
    blobField.value = data;
    
    result.fields.push_back(blobField);
    return result;
}

} // namespace rdx::core


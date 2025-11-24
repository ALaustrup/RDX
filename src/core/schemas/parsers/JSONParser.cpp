#include "schemas/parsers/JSONParser.h"
#include <fstream>
#include <sstream>
#include <cctype>

namespace rdx::core {

JSONParser::JSONParser(const SchemaDefinition& schema)
    : schema_(schema) {
}

bool JSONParser::canParse(const DetectedFileType& fileType,
                          std::span<const std::byte> prefix) const {
    if (fileType.name == "json") {
        return true;
    }
    
    // Check if starts with JSON-like characters
    if (prefix.size() > 0) {
        std::byte first = prefix[0];
        return first == std::byte{'{'} || first == std::byte{'['};
    }
    
    return false;
}

ParsedRepresentation JSONParser::parse(const std::filesystem::path& path) {
    ParsedRepresentation result;
    result.schema = &schema_;
    
    // Read file as string
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string jsonStr = oss.str();
    
    // Simplified JSON parsing (in production, use a proper JSON library like nlohmann/json)
    // For now, just store as a string value
    ParsedField jsonField;
    jsonField.name = "json_value";
    jsonField.kind = FieldKind::String;
    
    // Store as string (no need to convert to bytes here)
    jsonField.value = std::string(jsonStr);
    
    result.fields.push_back(jsonField);
    return result;
}

} // namespace rdx::core


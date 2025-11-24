#include "schemas/parsers/KVConfigParser.h"
#include <fstream>
#include <regex>

namespace rdx::core {

KVConfigParser::KVConfigParser(const SchemaDefinition& schema)
    : schema_(schema) {
}

bool KVConfigParser::canParse(const DetectedFileType& fileType,
                              std::span<const std::byte> prefix) const {
    return fileType.name == "kv_config";
}

ParsedRepresentation KVConfigParser::parse(const std::filesystem::path& path) {
    ParsedRepresentation result;
    result.schema = &schema_;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    
    std::string currentSection = "";
    std::string line;
    std::regex sectionRegex(R"(\[([^\]]+)\])");
    std::regex keyValueRegex(R"(([^=]+)=(.+))");
    
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        std::smatch match;
        
        // Check for section
        if (std::regex_match(line, match, sectionRegex)) {
            currentSection = match[1].str();
            continue;
        }
        
        // Check for key=value
        if (std::regex_match(line, match, keyValueRegex)) {
            ParsedField kvEntry;
            kvEntry.name = "kv_entry";
            kvEntry.kind = FieldKind::Record;
            
            ParsedField section;
            section.name = "section";
            section.kind = FieldKind::String;
            section.value = currentSection;
            
            ParsedField key;
            key.name = "key";
            key.kind = FieldKind::String;
            key.value = match[1].str();
            
            ParsedField value;
            value.name = "value";
            value.kind = FieldKind::String;
            value.value = match[2].str();
            
            kvEntry.value = std::vector<ParsedField>{section, key, value};
            result.fields.push_back(kvEntry);
        }
    }
    
    return result;
}

} // namespace rdx::core


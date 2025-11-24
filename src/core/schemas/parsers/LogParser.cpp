#include "schemas/parsers/LogParser.h"
#include <fstream>
#include <sstream>
#include <regex>

namespace rdx::core {

LogParser::LogParser(const SchemaDefinition& schema)
    : schema_(schema) {
}

bool LogParser::canParse(const DetectedFileType& fileType,
                         std::span<const std::byte> prefix) const {
    return fileType.name == "log_line";
}

ParsedRepresentation LogParser::parse(const std::filesystem::path& path) {
    ParsedRepresentation result;
    result.schema = &schema_;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    
    std::string line;
    std::regex logRegex(R"((\d{4}-\d{2}-\d{2}[\sT]\d{2}:\d{2}:\d{2})\s+(\w+)\s+\[(\w+)\]\s+(.*))");
    
    while (std::getline(file, line)) {
        ParsedField logLine;
        logLine.name = "log_line";
        logLine.kind = FieldKind::Record;
        
        std::smatch match;
        if (std::regex_match(line, match, logRegex)) {
            ParsedField timestamp;
            timestamp.name = "timestamp";
            timestamp.kind = FieldKind::String;
            timestamp.value = match[1].str();
            
            ParsedField level;
            level.name = "level";
            level.kind = FieldKind::Enum;
            level.value = match[2].str();
            
            ParsedField component;
            component.name = "component";
            component.kind = FieldKind::String;
            component.value = match[3].str();
            
            ParsedField message;
            message.name = "message";
            message.kind = FieldKind::String;
            message.value = match[4].str();
            
            logLine.value = std::vector<ParsedField>{timestamp, level, component, message};
        } else {
            // Fallback: store as raw message
            ParsedField message;
            message.name = "message";
            message.kind = FieldKind::String;
            message.value = line;
            logLine.value = std::vector<ParsedField>{message};
        }
        
        result.fields.push_back(logLine);
    }
    
    return result;
}

} // namespace rdx::core


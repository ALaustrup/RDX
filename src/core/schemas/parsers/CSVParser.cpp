#include "schemas/parsers/CSVParser.h"
#include <fstream>
#include <sstream>

namespace rdx::core {

CSVParser::CSVParser(const SchemaDefinition& schema)
    : schema_(schema) {
}

bool CSVParser::canParse(const DetectedFileType& fileType,
                         std::span<const std::byte> prefix) const {
    return fileType.name == "csv_simple";
}

std::vector<std::string> CSVParser::splitCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    
    while (std::getline(ss, field, ',')) {
        fields.push_back(field);
    }
    
    return fields;
}

ParsedRepresentation CSVParser::parse(const std::filesystem::path& path) {
    ParsedRepresentation result;
    result.schema = &schema_;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    
    // Read header
    std::string headerLine;
    if (!std::getline(file, headerLine)) {
        return result;
    }
    
    auto headers = splitCSVLine(headerLine);
    
    ParsedField csvTable;
    csvTable.name = "csv_table";
    csvTable.kind = FieldKind::Record;
    
    // Store header
    ParsedField header;
    header.name = "header";
    header.kind = FieldKind::Array;
    std::vector<ParsedField> headerFields;
    for (const auto& h : headers) {
        ParsedField f;
        f.name = "header_field";
        f.kind = FieldKind::String;
        f.value = h;
        headerFields.push_back(f);
    }
    header.value = headerFields;
    
    // Read rows
    ParsedField rows;
    rows.name = "rows";
    rows.kind = FieldKind::Array;
    std::vector<ParsedField> rowFields;
    
    std::string line;
    while (std::getline(file, line)) {
        auto values = splitCSVLine(line);
        if (values.size() == headers.size()) {
            ParsedField row;
            row.name = "row";
            row.kind = FieldKind::Record;
            std::vector<ParsedField> rowValues;
            
            for (std::size_t i = 0; i < values.size() && i < headers.size(); ++i) {
                ParsedField cell;
                cell.name = headers[i];
                cell.kind = FieldKind::String;
                cell.value = values[i];
                rowValues.push_back(cell);
            }
            
            row.value = rowValues;
            rowFields.push_back(row);
        }
    }
    
    rows.value = rowFields;
    csvTable.value = std::vector<ParsedField>{header, rows};
    result.fields.push_back(csvTable);
    
    return result;
}

} // namespace rdx::core


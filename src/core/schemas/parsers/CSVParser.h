#ifndef RDX_CSVPARSER_H
#define RDX_CSVPARSER_H

#include "schemas/parsers/ISchemaParser.h"

namespace rdx::core {

class CSVParser : public ISchemaParser {
public:
    CSVParser(const SchemaDefinition& schema);
    
    bool canParse(const DetectedFileType& fileType, 
                  std::span<const std::byte> prefix) const override;
    
    ParsedRepresentation parse(const std::filesystem::path& path) override;
    
    const SchemaDefinition& getSchema() const override { return schema_; }

private:
    const SchemaDefinition& schema_;
    
    std::vector<std::string> splitCSVLine(const std::string& line);
};

} // namespace rdx::core

#endif // RDX_CSVPARSER_H


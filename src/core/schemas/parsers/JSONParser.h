#ifndef RDX_JSONPARSER_H
#define RDX_JSONPARSER_H

#include "schemas/parsers/ISchemaParser.h"

namespace rdx::core {

class JSONParser : public ISchemaParser {
public:
    JSONParser(const SchemaDefinition& schema);
    
    bool canParse(const DetectedFileType& fileType, 
                  std::span<const std::byte> prefix) const override;
    
    ParsedRepresentation parse(const std::filesystem::path& path) override;
    
    const SchemaDefinition& getSchema() const override { return schema_; }

private:
    const SchemaDefinition& schema_;
    
    ParsedField parseJSONValue(const std::string& jsonStr, std::size_t& pos);
};

} // namespace rdx::core

#endif // RDX_JSONPARSER_H


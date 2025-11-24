#ifndef RDX_LOGPARSER_H
#define RDX_LOGPARSER_H

#include "schemas/parsers/ISchemaParser.h"

namespace rdx::core {

class LogParser : public ISchemaParser {
public:
    LogParser(const SchemaDefinition& schema);
    
    bool canParse(const DetectedFileType& fileType, 
                  std::span<const std::byte> prefix) const override;
    
    ParsedRepresentation parse(const std::filesystem::path& path) override;
    
    const SchemaDefinition& getSchema() const override { return schema_; }

private:
    const SchemaDefinition& schema_;
};

} // namespace rdx::core

#endif // RDX_LOGPARSER_H


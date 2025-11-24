#ifndef RDX_PE32PARSER_H
#define RDX_PE32PARSER_H

#include "schemas/parsers/ISchemaParser.h"

namespace rdx::core {

class PE32Parser : public ISchemaParser {
public:
    PE32Parser(const SchemaDefinition& schema);
    
    bool canParse(const DetectedFileType& fileType, 
                  std::span<const std::byte> prefix) const override;
    
    ParsedRepresentation parse(const std::filesystem::path& path) override;
    
    const SchemaDefinition& getSchema() const override { return schema_; }

private:
    const SchemaDefinition& schema_;
    
    std::uint16_t readU16(std::span<const std::byte> data, std::size_t offset) const;
    std::uint32_t readU32(std::span<const std::byte> data, std::size_t offset) const;
};

} // namespace rdx::core

#endif // RDX_PE32PARSER_H


#ifndef RDX_ISCHEMAPARSER_H
#define RDX_ISCHEMAPARSER_H

#include "schemas/SchemaDefinition.h"
#include "detectors/FileTypeDetector.h"
#include <filesystem>
#include <span>
#include <cstddef>
#include <memory>
#include <variant>
#include <vector>
#include <map>
#include <string>

namespace rdx::core {

// Intermediate representation of parsed data
struct ParsedField {
    std::string name;
    FieldKind kind;
    std::variant<
        std::int64_t,           // Integer
        std::string,            // String/Enum
        std::vector<std::byte>, // Bytes
        std::vector<ParsedField> // Record/Array
    > value;
};

struct ParsedRepresentation {
    const SchemaDefinition* schema;
    std::vector<ParsedField> fields;
    std::map<std::string, std::string> metadata;
    
    // Constraint graph (simplified - in production, use proper graph structure)
    std::map<std::string, std::vector<std::string>> constraints;  // field -> [referenced fields]
};

class ISchemaParser {
public:
    virtual ~ISchemaParser() = default;
    
    virtual bool canParse(const DetectedFileType& fileType, 
                         std::span<const std::byte> prefix) const = 0;
    
    virtual ParsedRepresentation parse(const std::filesystem::path& path) = 0;
    
    virtual const SchemaDefinition& getSchema() const = 0;
};

} // namespace rdx::core

#endif // RDX_ISCHEMAPARSER_H


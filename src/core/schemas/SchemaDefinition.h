#ifndef RDX_SCHEMADEFINITION_H
#define RDX_SCHEMADEFINITION_H

#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <map>
#include <cstdint>

namespace rdx::core {

enum class FieldKind {
    Integer,
    Enum,
    String,
    Bytes,
    LengthOf,
    ChecksumOf,
    OffsetOf,
    Record,
    Array,
    Section
};

struct FieldConstraint {
    enum Type {
        Range,           // min/max values
        Reference,       // references another field
        Checksum,        // must match checksum
        Alignment,       // byte alignment requirement
        Enumeration      // enum values
    };
    
    Type type;
    std::variant<
        std::pair<std::int64_t, std::int64_t>,  // Range: (min, max)
        std::string,                             // Reference: field name
        std::vector<std::string>                 // Enumeration: allowed values
    > value;
};

struct FieldDefinition {
    std::string name;
    FieldKind kind;
    std::optional<std::string> refersTo;  // for len_of, checksum_of, offset_of
    std::vector<FieldConstraint> constraints;
    std::optional<std::int32_t> sizeBytes;  // fixed size for some types
    std::optional<std::string> encoding;    // e.g., "utf-8", "ascii"
    
    // For nested structures
    std::vector<FieldDefinition> nestedFields;  // for Record/Array
};

struct SchemaDefinition {
    int schemaId;
    std::string name;
    int version;
    std::vector<FieldDefinition> fields;
    std::map<std::string, std::string> metadata;  // additional schema metadata
    
    // Find field by name
    const FieldDefinition* findField(const std::string& name) const;
    FieldDefinition* findField(const std::string& name);
    
    // Validate structure
    bool validate() const;
};

} // namespace rdx::core

#endif // RDX_SCHEMADEFINITION_H


#include "schemas/SchemaDefinition.h"
#include <algorithm>

namespace rdx::core {

const FieldDefinition* SchemaDefinition::findField(const std::string& name) const {
    for (const auto& field : fields) {
        if (field.name == name) {
            return &field;
        }
        // Search nested fields
        for (const auto& nested : field.nestedFields) {
            if (nested.name == name) {
                return &nested;
            }
        }
    }
    return nullptr;
}

FieldDefinition* SchemaDefinition::findField(const std::string& name) {
    return const_cast<FieldDefinition*>(const_cast<const SchemaDefinition*>(this)->findField(name));
}

bool SchemaDefinition::validate() const {
    // Basic validation: check for required fields, constraint consistency, etc.
    if (name.empty()) {
        return false;
    }
    
    // Check that referenced fields exist
    for (const auto& field : fields) {
        if (field.refersTo.has_value()) {
            if (!findField(field.refersTo.value())) {
                return false;
            }
        }
        
        // Validate nested fields recursively
        for (const auto& nested : field.nestedFields) {
            if (nested.refersTo.has_value()) {
                if (!findField(nested.refersTo.value())) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

} // namespace rdx::core


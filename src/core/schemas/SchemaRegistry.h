#ifndef RDX_SCHEMAREGISTRY_H
#define RDX_SCHEMAREGISTRY_H

#include "schemas/SchemaDefinition.h"
#include "lcm/LCMManager.h"
#include <vector>
#include <memory>
#include <map>

namespace rdx::core {

class SchemaRegistry {
public:
    explicit SchemaRegistry(LCMManager& lcm);
    ~SchemaRegistry() = default;
    
    // Get schema by ID
    const SchemaDefinition& getSchemaById(int schemaId) const;
    
    // Get default schema for file type
    const SchemaDefinition& getDefaultSchemaForFileType(int fileTypeId) const;
    
    // List all registered schemas
    std::vector<SchemaDefinition> listAllSchemas() const;
    
    // Get schema ID by name and version
    int getSchemaId(const std::string& name, int version) const;
    
    // Register a new schema (adds to LCM)
    int registerSchema(const SchemaDefinition& schema);
    
    // Schema constants
    static constexpr int SCHEMA_PE32_ID = 1;
    static constexpr int SCHEMA_JSON_GENERIC_ID = 2;
    static constexpr int SCHEMA_LOG_LINE_ID = 3;
    static constexpr int SCHEMA_CSV_SIMPLE_ID = 4;
    static constexpr int SCHEMA_KV_CONFIG_ID = 5;
    static constexpr int SCHEMA_CHUNKED_BINARY_ID = 6;
    static constexpr int SCHEMA_UNSTRUCTURED_BINARY_ID = 7;

private:
    LCMManager& lcm_;
    std::map<int, SchemaDefinition> schemas_;
    
    void initializeBuiltinSchemas();
    SchemaDefinition createPE32Schema();
    SchemaDefinition createJSONGenericSchema();
    SchemaDefinition createLogLineSchema();
    SchemaDefinition createCSVSimpleSchema();
    SchemaDefinition createKVConfigSchema();
    SchemaDefinition createChunkedBinarySchema();
    SchemaDefinition createUnstructuredBinarySchema();
    
    std::string schemaToJSON(const SchemaDefinition& schema) const;
    SchemaDefinition schemaFromJSON(const std::string& json) const;
};

} // namespace rdx::core

#endif // RDX_SCHEMAREGISTRY_H


#include "schemas/SchemaRegistry.h"
#include <sstream>
#include <stdexcept>

namespace rdx::core {

SchemaRegistry::SchemaRegistry(LCMManager& lcm) : lcm_(lcm) {
    initializeBuiltinSchemas();
}

void SchemaRegistry::initializeBuiltinSchemas() {
    // Create and register built-in schemas
    auto pe32 = createPE32Schema();
    auto json = createJSONGenericSchema();
    auto log = createLogLineSchema();
    auto csv = createCSVSimpleSchema();
    auto kv = createKVConfigSchema();
    auto chunked = createChunkedBinarySchema();
    auto unstructured = createUnstructuredBinarySchema();
    
    // Register in LCM and store locally
    pe32.schemaId = registerSchema(pe32);
    json.schemaId = registerSchema(json);
    log.schemaId = registerSchema(log);
    csv.schemaId = registerSchema(csv);
    kv.schemaId = registerSchema(kv);
    chunked.schemaId = registerSchema(chunked);
    unstructured.schemaId = registerSchema(unstructured);
    
    schemas_[pe32.schemaId] = pe32;
    schemas_[json.schemaId] = json;
    schemas_[log.schemaId] = log;
    schemas_[csv.schemaId] = csv;
    schemas_[kv.schemaId] = kv;
    schemas_[chunked.schemaId] = chunked;
    schemas_[unstructured.schemaId] = unstructured;
}

const SchemaDefinition& SchemaRegistry::getSchemaById(int schemaId) const {
    auto it = schemas_.find(schemaId);
    if (it != schemas_.end()) {
        return it->second;
    }
    
    // Try to load from LCM
    auto def = lcm_.loadSchemaDefinition(schemaId);
    if (def.has_value()) {
        // Parse and cache (simplified - in production, properly parse JSON)
        throw std::runtime_error("Schema loading from LCM not fully implemented");
    }
    
    throw std::runtime_error("Schema not found: " + std::to_string(schemaId));
}

const SchemaDefinition& SchemaRegistry::getDefaultSchemaForFileType(int fileTypeId) const {
    std::string typeName = lcm_.getFileTypeName(fileTypeId);
    
    // Map file types to schemas
    if (typeName == "pe32" || typeName == "pe64") {
        return getSchemaById(SCHEMA_PE32_ID);
    } else if (typeName == "json") {
        return getSchemaById(SCHEMA_JSON_GENERIC_ID);
    } else if (typeName == "log_line") {
        return getSchemaById(SCHEMA_LOG_LINE_ID);
    } else if (typeName == "csv_simple") {
        return getSchemaById(SCHEMA_CSV_SIMPLE_ID);
    } else if (typeName == "kv_config") {
        return getSchemaById(SCHEMA_KV_CONFIG_ID);
    } else if (typeName == "chunked_binary") {
        return getSchemaById(SCHEMA_CHUNKED_BINARY_ID);
    }
    
    // Default to unstructured
    return getSchemaById(SCHEMA_UNSTRUCTURED_BINARY_ID);
}

std::vector<SchemaDefinition> SchemaRegistry::listAllSchemas() const {
    std::vector<SchemaDefinition> result;
    for (const auto& [id, schema] : schemas_) {
        result.push_back(schema);
    }
    return result;
}

int SchemaRegistry::getSchemaId(const std::string& name, int version) const {
    for (const auto& [id, schema] : schemas_) {
        if (schema.name == name && schema.version == version) {
            return id;
        }
    }
    return -1;
}

int SchemaRegistry::registerSchema(const SchemaDefinition& schema) {
    std::string jsonDef = schemaToJSON(schema);
    int schemaId = lcm_.getOrCreateSchemaId(schema.name, schema.version, jsonDef);
    
    // Update local cache
    SchemaDefinition cached = schema;
    cached.schemaId = schemaId;
    schemas_[schemaId] = cached;
    
    return schemaId;
}

SchemaDefinition SchemaRegistry::createPE32Schema() {
    SchemaDefinition schema;
    schema.name = "PE32";
    schema.version = 1;
    
    // DOS Header
    FieldDefinition dosHeader;
    dosHeader.name = "dos_header";
    dosHeader.kind = FieldKind::Record;
    {
        FieldDefinition f;
        f.name = "e_magic"; f.kind = FieldKind::Integer; f.sizeBytes = 2;
        dosHeader.nestedFields.push_back(f);
        f.name = "e_lfanew"; f.kind = FieldKind::Integer; f.sizeBytes = 4;
        dosHeader.nestedFields.push_back(f);
        // Add other DOS header fields as needed
    }
    
    // PE Header
    FieldDefinition peHeader;
    peHeader.name = "pe_header";
    peHeader.kind = FieldKind::Record;
    {
        FieldDefinition f;
        f.name = "signature"; f.kind = FieldKind::Integer; f.sizeBytes = 4;
        peHeader.nestedFields.push_back(f);
        f.name = "machine"; f.kind = FieldKind::Integer; f.sizeBytes = 2;
        peHeader.nestedFields.push_back(f);
        f.name = "number_of_sections"; f.kind = FieldKind::Integer; f.sizeBytes = 2;
        peHeader.nestedFields.push_back(f);
        // Add other PE header fields as needed
    }
    
    // Sections array
    FieldDefinition sections;
    sections.name = "sections";
    sections.kind = FieldKind::Array;
    {
        FieldDefinition f;
        f.name = "name"; f.kind = FieldKind::String; f.sizeBytes = 8;
        sections.nestedFields.push_back(f);
        f.name = "size_of_raw_data"; f.kind = FieldKind::Integer; f.sizeBytes = 4;
        sections.nestedFields.push_back(f);
        f.name = "raw_data"; f.kind = FieldKind::Bytes; f.refersTo = "size_of_raw_data";
        sections.nestedFields.push_back(f);
        // Add other section fields as needed
    }
    
    schema.fields = {dosHeader, peHeader, sections};
    return schema;
}

SchemaDefinition SchemaRegistry::createJSONGenericSchema() {
    SchemaDefinition schema;
    schema.name = "JSON_GENERIC";
    schema.version = 1;
    
    // JSON is represented as a tree structure
    FieldDefinition jsonValue;
    jsonValue.name = "json_value";
    jsonValue.kind = FieldKind::Record;
    jsonValue.nestedFields = {
        {"type", FieldKind::Enum, std::nullopt, {}, 1},  // object, array, string, number, bool, null
        {"value", FieldKind::Bytes, std::nullopt, {}}    // encoded value
    };
    
    schema.fields = {jsonValue};
    return schema;
}

SchemaDefinition SchemaRegistry::createLogLineSchema() {
    SchemaDefinition schema;
    schema.name = "LOG_LINE";
    schema.version = 1;
    
    FieldDefinition logLine;
    logLine.name = "log_line";
    logLine.kind = FieldKind::Record;
    {
        FieldDefinition f;
        f.name = "timestamp"; f.kind = FieldKind::String; f.encoding = "iso8601";
        logLine.nestedFields.push_back(f);
        f.name = "level"; f.kind = FieldKind::Enum; f.encoding = std::nullopt;
        logLine.nestedFields.push_back(f);
        f.name = "component"; f.kind = FieldKind::String;
        logLine.nestedFields.push_back(f);
        f.name = "message"; f.kind = FieldKind::String;
        logLine.nestedFields.push_back(f);
    }
    
    schema.fields = {logLine};
    return schema;
}

SchemaDefinition SchemaRegistry::createCSVSimpleSchema() {
    SchemaDefinition schema;
    schema.name = "CSV_SIMPLE";
    schema.version = 1;
    
    FieldDefinition csvTable;
    csvTable.name = "csv_table";
    csvTable.kind = FieldKind::Record;
    {
        FieldDefinition f;
        f.name = "header"; f.kind = FieldKind::Array;
        csvTable.nestedFields.push_back(f);
        f.name = "rows"; f.kind = FieldKind::Array;
        csvTable.nestedFields.push_back(f);
    }
    
    schema.fields = {csvTable};
    return schema;
}

SchemaDefinition SchemaRegistry::createKVConfigSchema() {
    SchemaDefinition schema;
    schema.name = "KV_CONFIG";
    schema.version = 1;
    
    FieldDefinition kvEntry;
    kvEntry.name = "kv_entry";
    kvEntry.kind = FieldKind::Record;
    {
        FieldDefinition f;
        f.name = "section"; f.kind = FieldKind::String;
        kvEntry.nestedFields.push_back(f);
        f.name = "key"; f.kind = FieldKind::String;
        kvEntry.nestedFields.push_back(f);
        f.name = "value"; f.kind = FieldKind::String;
        kvEntry.nestedFields.push_back(f);
    }
    
    schema.fields = {kvEntry};
    return schema;
}

SchemaDefinition SchemaRegistry::createChunkedBinarySchema() {
    SchemaDefinition schema;
    schema.name = "CHUNKED_BINARY";
    schema.version = 1;
    
    FieldDefinition chunk;
    chunk.name = "chunk";
    chunk.kind = FieldKind::Record;
    {
        FieldDefinition f;
        f.name = "type_id"; f.kind = FieldKind::Integer; f.sizeBytes = 4;
        chunk.nestedFields.push_back(f);
        f.name = "length"; f.kind = FieldKind::Integer; f.sizeBytes = 4;
        chunk.nestedFields.push_back(f);
        f.name = "payload"; f.kind = FieldKind::Bytes; f.refersTo = "length";
        chunk.nestedFields.push_back(f);
    }
    
    schema.fields = {chunk};
    return schema;
}

SchemaDefinition SchemaRegistry::createUnstructuredBinarySchema() {
    SchemaDefinition schema;
    schema.name = "UNSTRUCTURED_BINARY";
    schema.version = 1;
    
    FieldDefinition blob;
    blob.name = "blob";
    blob.kind = FieldKind::Bytes;
    
    schema.fields = {blob};
    return schema;
}

std::string SchemaRegistry::schemaToJSON(const SchemaDefinition& schema) const {
    // Simplified JSON serialization (in production, use proper JSON library)
    std::ostringstream oss;
    oss << "{\"name\":\"" << schema.name << "\",\"version\":" << schema.version << "}";
    return oss.str();
}

SchemaDefinition SchemaRegistry::schemaFromJSON(const std::string& json) const {
    // Simplified JSON deserialization (in production, use proper JSON library)
    SchemaDefinition schema;
    // Parse JSON and populate schema
    return schema;
}

} // namespace rdx::core


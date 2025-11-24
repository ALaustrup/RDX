# Schema Definition Language (DSL)

RDX uses a C++-based schema definition system to describe the structure and constraints of different file formats.

## SchemaDefinition Structure

A `SchemaDefinition` consists of:

- **schemaId**: Unique identifier (assigned by LCM)
- **name**: Schema name (e.g., "PE32", "JSON_GENERIC")
- **version**: Schema version number
- **fields**: Vector of `FieldDefinition` objects
- **metadata**: Map of additional metadata

## FieldDefinition Structure

Each field has:

- **name**: Field name (unique within schema)
- **kind**: Field kind (see below)
- **refersTo**: Optional reference to another field (for length_of, checksum_of, offset_of)
- **constraints**: Vector of `FieldConstraint` objects
- **sizeBytes**: Optional fixed size in bytes
- **encoding**: Optional encoding (e.g., "utf-8", "iso8601")
- **nestedFields**: For Record/Array kinds, contains child fields

## Field Kinds

### Integer

Fixed-size integer fields.

```cpp
FieldDefinition field;
field.name = "size";
field.kind = FieldKind::Integer;
field.sizeBytes = 4;  // 32-bit integer
```

### Enum

Enumeration field with allowed values.

```cpp
FieldDefinition field;
field.name = "level";
field.kind = FieldKind::Enum;
// Constraints define allowed values
```

### String

Variable-length string field.

```cpp
FieldDefinition field;
field.name = "message";
field.kind = FieldKind::String;
field.encoding = "utf-8";
```

### Bytes

Binary data field.

```cpp
FieldDefinition field;
field.name = "raw_data";
field.kind = FieldKind::Bytes;
field.refersTo = "size_of_raw_data";  // Length from another field
```

### LengthOf

Derived field that is the length of another field.

```cpp
FieldDefinition field;
field.name = "name_length";
field.kind = FieldKind::LengthOf;
field.refersTo = "name";
```

### ChecksumOf

Derived field that is the checksum of another field or region.

```cpp
FieldDefinition field;
field.name = "header_checksum";
field.kind = FieldKind::ChecksumOf;
field.refersTo = "header";
```

### OffsetOf

Derived field that is the offset of another field.

```cpp
FieldDefinition field;
field.name = "data_offset";
field.kind = FieldKind::OffsetOf;
field.refersTo = "data";
```

### Record

Structured record containing nested fields.

```cpp
FieldDefinition record;
record.name = "dos_header";
record.kind = FieldKind::Record;
record.nestedFields = {
    {"e_magic", FieldKind::Integer, std::nullopt, {}, 2},
    {"e_lfanew", FieldKind::Integer, std::nullopt, {}, 4}
};
```

### Array

Array of elements (all same type or record).

```cpp
FieldDefinition array;
array.name = "sections";
array.kind = FieldKind::Array;
array.nestedFields = {
    {"name", FieldKind::String, std::nullopt, {}, 8},
    {"size", FieldKind::Integer, std::nullopt, {}, 4}
};
```

### Section

Named section (similar to record, but with semantic meaning).

```cpp
FieldDefinition section;
section.name = ".text";
section.kind = FieldKind::Section;
section.nestedFields = { /* section fields */ };
```

## Constraints

Constraints provide additional validation and encoding hints:

### Range Constraint

```cpp
FieldConstraint constraint;
constraint.type = FieldConstraint::Range;
constraint.value = std::make_pair(0, 65535);  // min, max
```

### Reference Constraint

```cpp
FieldConstraint constraint;
constraint.type = FieldConstraint::Reference;
constraint.value = std::string("other_field");
```

### Enumeration Constraint

```cpp
FieldConstraint constraint;
constraint.type = FieldConstraint::Enumeration;
constraint.value = std::vector<std::string>{"DEBUG", "INFO", "WARN", "ERROR"};
```

## Example: PE32 Schema

```cpp
SchemaDefinition pe32;
pe32.name = "PE32";
pe32.version = 1;

// DOS Header
FieldDefinition dosHeader;
dosHeader.name = "dos_header";
dosHeader.kind = FieldKind::Record;
dosHeader.nestedFields = {
    {"e_magic", FieldKind::Integer, std::nullopt, {}, 2},
    {"e_lfanew", FieldKind::Integer, std::nullopt, {}, 4}  // offset to PE header
};

// PE Header
FieldDefinition peHeader;
peHeader.name = "pe_header";
peHeader.kind = FieldKind::Record;
peHeader.nestedFields = {
    {"signature", FieldKind::Integer, std::nullopt, {}, 4},
    {"number_of_sections", FieldKind::Integer, std::nullopt, {}, 2}
};

// Sections array
FieldDefinition sections;
sections.name = "sections";
sections.kind = FieldKind::Array;
sections.nestedFields = {
    {"name", FieldKind::String, std::nullopt, {}, 8},
    {"size_of_raw_data", FieldKind::Integer, std::nullopt, {}, 4},
    {"raw_data", FieldKind::Bytes, "size_of_raw_data", {}}
};

pe32.fields = {dosHeader, peHeader, sections};
```

## Adding New Schemas

To add a new schema:

1. Create `SchemaDefinition` with fields
2. Implement corresponding parser in `src/core/schemas/parsers/`
3. Register schema in `SchemaRegistry::initializeBuiltinSchemas()`
4. Add schema constant to `SchemaRegistry`

## Schema Serialization

Schemas are serialized to JSON for storage in LCM:

```json
{
  "name": "PE32",
  "version": 1,
  "fields": [
    {
      "name": "dos_header",
      "kind": "Record",
      "nestedFields": [...]
    }
  ]
}
```

## Constraint Graph

The constraint graph describes field dependencies:

- `length_of(field)` → depends on `field`
- `checksum_of(field)` → depends on `field`
- `offset_of(field)` → depends on `field`

During decompression, the canonical rebuilder uses the constraint graph to:
1. Decode independent fields first
2. Use constraints to derive dependent fields
3. Reconstruct exact bytes in correct order


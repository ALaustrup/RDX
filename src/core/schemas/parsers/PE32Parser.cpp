#include "schemas/parsers/PE32Parser.h"
#include <fstream>
#include <cstring>

namespace rdx::core {

PE32Parser::PE32Parser(const SchemaDefinition& schema)
    : schema_(schema) {
}

bool PE32Parser::canParse(const DetectedFileType& fileType,
                          std::span<const std::byte> prefix) const {
    if (fileType.name == "pe32") {
        return true;
    }
    
    // Check for MZ signature
    if (prefix.size() >= 2) {
        return prefix[0] == std::byte{0x4D} && prefix[1] == std::byte{0x5A};
    }
    
    return false;
}

std::uint16_t PE32Parser::readU16(std::span<const std::byte> data, std::size_t offset) const {
    if (offset + 2 > data.size()) {
        return 0;
    }
    std::uint16_t value;
    std::memcpy(&value, data.data() + offset, 2);
    return value;
}

std::uint32_t PE32Parser::readU32(std::span<const std::byte> data, std::size_t offset) const {
    if (offset + 4 > data.size()) {
        return 0;
    }
    std::uint32_t value;
    std::memcpy(&value, data.data() + offset, 4);
    return value;
}

ParsedRepresentation PE32Parser::parse(const std::filesystem::path& path) {
    ParsedRepresentation result;
    result.schema = &schema_;
    
    // Read entire file
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    
    std::size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<std::byte> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    // Parse DOS header
    ParsedField dosHeader;
    dosHeader.name = "dos_header";
    dosHeader.kind = FieldKind::Record;
    
    ParsedField eMagic;
    eMagic.name = "e_magic";
    eMagic.kind = FieldKind::Integer;
    eMagic.value = static_cast<std::int64_t>(readU16(data, 0));
    dosHeader.value = std::vector<ParsedField>{eMagic};
    
    ParsedField eLfanew;
    eLfanew.name = "e_lfanew";
    eLfanew.kind = FieldKind::Integer;
    eLfanew.value = static_cast<std::int64_t>(readU32(data, 60));
    std::get<std::vector<ParsedField>>(dosHeader.value).push_back(eLfanew);
    
    result.fields.push_back(dosHeader);
    
    // Parse PE header if available
    std::uint32_t peOffset = readU32(data, 60);
    if (peOffset < data.size() && peOffset + 4 <= data.size()) {
        std::uint32_t peSignature = readU32(data, peOffset);
        if (peSignature == 0x00004550) {  // "PE\0\0"
            ParsedField peHeader;
            peHeader.name = "pe_header";
            peHeader.kind = FieldKind::Record;
            
            ParsedField sig;
            sig.name = "signature";
            sig.kind = FieldKind::Integer;
            sig.value = static_cast<std::int64_t>(peSignature);
            peHeader.value = std::vector<ParsedField>{sig};
            
            result.fields.push_back(peHeader);
        }
    }
    
    return result;
}

} // namespace rdx::core


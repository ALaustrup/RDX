#include "detectors/FileTypeDetector.h"
#include <algorithm>
#include <cstring>
#include <string_view>

namespace rdx::core {

DetectedFileType FileTypeDetector::detect(const std::filesystem::path& path,
                                           std::span<const std::byte> prefix) {
    // First try magic bytes
    auto result = detectFromMagicBytes(prefix);
    if (result.fileTypeId != -1) {
        return result;
    }
    
    // Fallback to extension
    return detectFromExtension(path);
}

DetectedFileType FileTypeDetector::detectFromExtension(const std::filesystem::path& path) {
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    DetectedFileType result;
    result.detectorSignature = "ext:" + ext;
    
    if (ext == ".exe" || ext == ".dll" || ext == ".sys" || ext == ".ocx" || ext == ".drv") {
        result.fileTypeId = 1;  // pe32
        result.name = "pe32";
    } else if (ext == ".json") {
        result.fileTypeId = 2;  // json
        result.name = "json";
    } else if (ext == ".log" || ext == ".txt") {
        result.fileTypeId = 3;  // log_line
        result.name = "log_line";
    } else if (ext == ".csv" || ext == ".tsv") {
        result.fileTypeId = 4;  // csv_simple
        result.name = "csv_simple";
    } else if (ext == ".ini" || ext == ".cfg" || ext == ".properties" || ext == ".conf" || ext == ".env") {
        result.fileTypeId = 5;  // kv_config
        result.name = "kv_config";
    } else {
        result.fileTypeId = 7;  // unstructured_binary
        result.name = "unstructured_binary";
    }
    
    return result;
}

DetectedFileType FileTypeDetector::detectFromMagicBytes(std::span<const std::byte> prefix) {
    DetectedFileType result;
    result.fileTypeId = -1;
    
    if (prefix.size() < 2) {
        return result;
    }
    
    // PE32/PE64: "MZ" signature
    if (prefix.size() >= 2) {
        if (prefix[0] == std::byte{0x4D} && prefix[1] == std::byte{0x5A}) {
            result.fileTypeId = 1;
            result.name = "pe32";
            result.detectorSignature = "magic:MZ";
            return result;
        }
    }
    
    // JSON: starts with '{' or '['
    if (prefix.size() >= 1) {
        if (prefix[0] == std::byte{'{'} || prefix[0] == std::byte{'['}) {
            result.fileTypeId = 2;
            result.name = "json";
            result.detectorSignature = "magic:json";
            return result;
        }
    }
    
    // ZIP (could be used for other formats): PK
    if (prefix.size() >= 2) {
        if (prefix[0] == std::byte{0x50} && prefix[1] == std::byte{0x4B}) {
            // Could be JAR, DOCX, etc. - treat as unstructured for now
            result.fileTypeId = 7;
            result.name = "unstructured_binary";
            result.detectorSignature = "magic:PK";
            return result;
        }
    }
    
    return result;
}

bool FileTypeDetector::checkMagicBytes(std::span<const std::byte> data,
                                        std::span<const std::byte> magic) const {
    if (data.size() < magic.size()) {
        return false;
    }
    
    return std::equal(magic.begin(), magic.end(), data.begin());
}

} // namespace rdx::core


#ifndef RDX_FILETYPEDETECTOR_H
#define RDX_FILETYPEDETECTOR_H

#include <filesystem>
#include <span>
#include <cstddef>
#include <string>

namespace rdx::core {

struct DetectedFileType {
    int fileTypeId;
    std::string name;
    std::string detectorSignature;
};

class FileTypeDetector {
public:
    DetectedFileType detect(const std::filesystem::path& path,
                            std::span<const std::byte> prefix);
    
    // Detect from extension only (fallback)
    DetectedFileType detectFromExtension(const std::filesystem::path& path);
    
    // Detect from magic bytes
    DetectedFileType detectFromMagicBytes(std::span<const std::byte> prefix);

private:
    bool checkMagicBytes(std::span<const std::byte> data, 
                         std::span<const std::byte> magic) const;
};

} // namespace rdx::core

#endif // RDX_FILETYPEDETECTOR_H


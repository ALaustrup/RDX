#ifndef RDX_HASHUTILS_H
#define RDX_HASHUTILS_H

#include <cstdint>
#include <string>
#include <span>
#include <cstddef>
#include <array>

namespace rdx::core {

// SHA-256 hash (64 bytes hex string)
std::string computeSHA256(std::span<const std::byte> data);

// SHA-256 hash as binary (32 bytes)
std::array<std::byte, 32> computeSHA256Binary(std::span<const std::byte> data);

// Fast hash for chunk fingerprints (64-bit)
std::uint64_t computeChunkFingerprint(std::span<const std::byte> data);

// Content hash for file deduplication
std::string computeContentHash(std::span<const std::byte> data);

// Path hash (for privacy-preserving indexing)
std::string computePathHash(const std::string& path);

} // namespace rdx::core

#endif // RDX_HASHUTILS_H


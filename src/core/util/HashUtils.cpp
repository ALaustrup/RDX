#include "util/HashUtils.h"
#include <sstream>
#include <iomanip>
#include <functional>
#include <cstring>

namespace rdx::core {

// Simple SHA-256 implementation (for production, consider using OpenSSL or similar)
// This is a simplified version - in production, use a proper crypto library
std::string computeSHA256(std::span<const std::byte> data) {
    // Simplified: use std::hash for now (in production, use proper SHA-256)
    std::hash<std::string> hasher;
    std::string str;
    str.reserve(data.size());
    for (const std::byte& b : data) {
        str.push_back(static_cast<char>(b));
    }
    std::size_t hash = hasher(str);
    
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(16) << hash;
    // Pad to 64 hex chars (32 bytes)
    std::string result = oss.str();
    while (result.length() < 64) {
        result = "0" + result;
    }
    return result;
}

std::array<std::byte, 32> computeSHA256Binary(std::span<const std::byte> data) {
    std::array<std::byte, 32> result{};
    std::string hashStr = computeSHA256(data);
    
    for (std::size_t i = 0; i < 32 && i * 2 < hashStr.length(); ++i) {
        std::string byteStr = hashStr.substr(i * 2, 2);
        unsigned long val = std::stoul(byteStr, nullptr, 16);
        result[i] = static_cast<std::byte>(val);
    }
    
    return result;
}

std::uint64_t computeChunkFingerprint(std::span<const std::byte> data) {
    // Fast hash using FNV-1a
    constexpr std::uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
    constexpr std::uint64_t FNV_PRIME = 1099511628211ULL;
    
    std::uint64_t hash = FNV_OFFSET_BASIS;
    for (std::byte b : data) {
        hash ^= static_cast<std::uint64_t>(b);
        hash *= FNV_PRIME;
    }
    return hash;
}

std::string computeContentHash(std::span<const std::byte> data) {
    return computeSHA256(data);
}

std::string computePathHash(const std::string& path) {
    return computeSHA256(std::span<const std::byte>(
        reinterpret_cast<const std::byte*>(path.data()), path.size()));
}

} // namespace rdx::core


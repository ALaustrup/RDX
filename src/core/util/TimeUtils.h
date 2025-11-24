#ifndef RDX_TIMEUTILS_H
#define RDX_TIMEUTILS_H

#include <cstdint>
#include <string>
#include <chrono>

namespace rdx::core {

// Get current Unix timestamp (seconds since epoch)
std::int64_t getCurrentTimestamp();

// Format timestamp as ISO 8601 string
std::string formatTimestamp(std::int64_t timestamp);

// Parse ISO 8601 string to timestamp
std::int64_t parseTimestamp(const std::string& isoString);

} // namespace rdx::core

#endif // RDX_TIMEUTILS_H


#include "util/TimeUtils.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace rdx::core {

std::int64_t getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

std::string formatTimestamp(std::int64_t timestamp) {
    std::time_t time = static_cast<std::time_t>(timestamp);
    std::tm* tm = std::gmtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::int64_t parseTimestamp(const std::string& isoString) {
    // Simplified parser - in production, use proper ISO 8601 parser
    std::tm tm = {};
    std::istringstream iss(isoString);
    iss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    
    if (iss.fail()) {
        return 0;
    }
    
    return std::mktime(&tm);
}

} // namespace rdx::core


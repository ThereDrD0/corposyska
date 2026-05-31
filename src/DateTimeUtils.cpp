#include "DateTimeUtils.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {
std::tm toUtcTm(std::time_t value) {
    std::tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &value);
#else
    gmtime_r(&value, &tm);
#endif
    return tm;
}

std::time_t fromUtcTm(std::tm* value) {
#if defined(_WIN32)
    return _mkgmtime(value);
#else
    return timegm(value);
#endif
}
}

namespace DateTimeUtils {

std::string formatDate(TimeEntry::TimePoint value) {
    std::time_t time = std::chrono::system_clock::to_time_t(value);
    std::tm tm = toUtcTm(time);
    std::ostringstream output;
    output << std::put_time(&tm, "%Y-%m-%d");
    return output.str();
}

std::string formatDateTime(TimeEntry::TimePoint value) {
    std::time_t time = std::chrono::system_clock::to_time_t(value);
    std::tm tm = toUtcTm(time);
    std::ostringstream output;
    output << std::put_time(&tm, "%Y-%m-%d %H:%M");
    return output.str();
}

TimeEntry::TimePoint parseDateTime(const std::string& value) {
    std::tm tm{};
    std::istringstream input(value);
    input >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    if (input.fail()) {
        throw std::invalid_argument("Expected date and time in format YYYY-MM-DD HH:MM");
    }
    std::time_t time = fromUtcTm(&tm);
    if (time == static_cast<std::time_t>(-1)) {
        throw std::invalid_argument("Invalid date and time");
    }
    return std::chrono::system_clock::from_time_t(time);
}

}

#pragma once

#include "TimeEntry.h"

#include <string>

namespace DateTimeUtils {

std::string formatDate(TimeEntry::TimePoint value);
std::string formatDateTime(TimeEntry::TimePoint value);
TimeEntry::TimePoint parseDateTime(const std::string& value);

}

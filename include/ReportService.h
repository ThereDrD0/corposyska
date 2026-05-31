#pragma once

#include "Task.h"
#include "TimeEntry.h"

#include <map>
#include <string>
#include <vector>

class ReportService {
public:
    std::map<std::string, long long> minutesByTask(
        const std::vector<Task>& tasks,
        const std::vector<TimeEntry>& entries
    ) const;

    std::map<std::string, long long> minutesByDay(
        const std::vector<TimeEntry>& entries
    ) const;
};

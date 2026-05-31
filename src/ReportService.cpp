#include "ReportService.h"
#include "DateTimeUtils.h"

#include <unordered_map>

std::map<std::string, long long> ReportService::minutesByTask(
    const std::vector<Task>& tasks,
    const std::vector<TimeEntry>& entries
) const {
    std::unordered_map<int, std::string> namesById;
    for (const Task& task : tasks) {
        namesById[task.id()] = task.name();
    }

    std::map<std::string, long long> result;
    for (const TimeEntry& entry : entries) {
        if (!entry.isClosed()) {
            continue;
        }
        auto found = namesById.find(entry.taskId());
        const std::string taskName = found == namesById.end() ? "Unknown task" : found->second;
        result[taskName] += entry.durationMinutes();
    }
    return result;
}

std::map<std::string, long long> ReportService::minutesByDay(
    const std::vector<TimeEntry>& entries
) const {
    std::map<std::string, long long> result;
    for (const TimeEntry& entry : entries) {
        if (!entry.isClosed()) {
            continue;
        }
        result[DateTimeUtils::formatDate(entry.start())] += entry.durationMinutes();
    }
    return result;
}

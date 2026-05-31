#pragma once

#include "Task.h"
#include "TimeEntry.h"

#include <string>
#include <vector>

class Storage {
public:
    Storage(std::string tasksPath, std::string entriesPath);

    std::vector<Task> loadTasks() const;
    std::vector<TimeEntry> loadEntries() const;
    void saveTasks(const std::vector<Task>& tasks) const;
    void saveEntries(const std::vector<TimeEntry>& entries) const;

private:
    std::string tasksPath_;
    std::string entriesPath_;
};

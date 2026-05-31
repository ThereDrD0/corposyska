#pragma once

#include "Task.h"
#include "TimeEntry.h"

#include <chrono>
#include <string>
#include <vector>

class TimeTrackerService {
public:
    using TimePoint = std::chrono::system_clock::time_point;

    int createTask(const std::string& name, const std::string& description = "");
    void addTask(const Task& task);
    void addEntry(const TimeEntry& entry);

    void startTask(int taskId, TimePoint start = std::chrono::system_clock::now());
    TimeEntry stopCurrentTask(TimePoint end = std::chrono::system_clock::now());
    void addManualEntry(int taskId, TimePoint start, TimePoint end);
    void archiveTask(int taskId);
    void renameTask(int taskId, const std::string& newName);

    const std::vector<Task>& tasks() const;
    const std::vector<TimeEntry>& entries() const;
    bool hasRunningTimer() const;

private:
    Task& findTask(int taskId);
    const Task& findTask(int taskId) const;
    int nextTaskId() const;
    int nextEntryId() const;

    std::vector<Task> tasks_;
    std::vector<TimeEntry> entries_;
    int runningEntryId_ = 0;
};

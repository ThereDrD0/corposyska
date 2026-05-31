#include "TimeTrackerService.h"

#include <algorithm>
#include <stdexcept>

int TimeTrackerService::createTask(const std::string& name, const std::string& description) {
    Task task(nextTaskId(), name, description);
    int id = task.id();
    tasks_.push_back(task);
    return id;
}

void TimeTrackerService::addTask(const Task& task) {
    auto it = std::find_if(tasks_.begin(), tasks_.end(), [&task](const Task& existing) {
        return existing.id() == task.id();
    });
    if (it != tasks_.end()) {
        throw std::logic_error("Task with the same id already exists");
    }
    tasks_.push_back(task);
}

void TimeTrackerService::addEntry(const TimeEntry& entry) {
    findTask(entry.taskId());
    entries_.push_back(entry);
}

void TimeTrackerService::startTask(int taskId, TimePoint start) {
    const Task& task = findTask(taskId);
    if (!task.isActive()) {
        throw std::logic_error("Archived task cannot be started");
    }
    if (hasRunningTimer()) {
        throw std::logic_error("Another timer is already running");
    }
    TimeEntry entry(nextEntryId(), taskId, start);
    runningEntryId_ = entry.id();
    entries_.push_back(entry);
}

TimeEntry TimeTrackerService::stopCurrentTask(TimePoint end) {
    if (!hasRunningTimer()) {
        throw std::logic_error("No running timer");
    }
    auto it = std::find_if(entries_.begin(), entries_.end(), [this](const TimeEntry& entry) {
        return entry.id() == runningEntryId_;
    });
    if (it == entries_.end()) {
        runningEntryId_ = 0;
        throw std::logic_error("Running timer entry was not found");
    }
    it->close(end);
    runningEntryId_ = 0;
    return *it;
}

void TimeTrackerService::addManualEntry(int taskId, TimePoint start, TimePoint end) {
    findTask(taskId);
    entries_.push_back(TimeEntry(nextEntryId(), taskId, start, end));
}

void TimeTrackerService::archiveTask(int taskId) {
    Task& task = findTask(taskId);
    task.markArchived();
}

void TimeTrackerService::renameTask(int taskId, const std::string& newName) {
    Task& task = findTask(taskId);
    task.rename(newName);
}

const std::vector<Task>& TimeTrackerService::tasks() const {
    return tasks_;
}

const std::vector<TimeEntry>& TimeTrackerService::entries() const {
    return entries_;
}

bool TimeTrackerService::hasRunningTimer() const {
    return runningEntryId_ != 0;
}

Task& TimeTrackerService::findTask(int taskId) {
    auto it = std::find_if(tasks_.begin(), tasks_.end(), [taskId](const Task& task) {
        return task.id() == taskId;
    });
    if (it == tasks_.end()) {
        throw std::out_of_range("Task was not found");
    }
    return *it;
}

const Task& TimeTrackerService::findTask(int taskId) const {
    auto it = std::find_if(tasks_.begin(), tasks_.end(), [taskId](const Task& task) {
        return task.id() == taskId;
    });
    if (it == tasks_.end()) {
        throw std::out_of_range("Task was not found");
    }
    return *it;
}

int TimeTrackerService::nextTaskId() const {
    int maxId = 0;
    for (const Task& task : tasks_) {
        maxId = std::max(maxId, task.id());
    }
    return maxId + 1;
}

int TimeTrackerService::nextEntryId() const {
    int maxId = 0;
    for (const TimeEntry& entry : entries_) {
        maxId = std::max(maxId, entry.id());
    }
    return maxId + 1;
}

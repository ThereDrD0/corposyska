#include "Storage.h"

#include <chrono>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
long long toSeconds(TimeEntry::TimePoint value) {
    return std::chrono::duration_cast<std::chrono::seconds>(value.time_since_epoch()).count();
}

TimeEntry::TimePoint fromSeconds(long long value) {
    return TimeEntry::TimePoint(std::chrono::seconds(value));
}

std::vector<std::string> split(const std::string& line, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(line);
    std::string item;
    while (std::getline(stream, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
}
}

Storage::Storage(std::string tasksPath, std::string entriesPath)
    : tasksPath_(std::move(tasksPath)), entriesPath_(std::move(entriesPath)) {}

std::vector<Task> Storage::loadTasks() const {
    std::vector<Task> tasks;
    std::ifstream input(tasksPath_);
    if (!input) {
        return tasks;
    }

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }
        std::vector<std::string> parts = split(line, ';');
        if (parts.size() < 4) {
            continue;
        }
        tasks.emplace_back(std::stoi(parts[0]), parts[1], parts[2], parts[3] == "1");
    }
    return tasks;
}

std::vector<TimeEntry> Storage::loadEntries() const {
    std::vector<TimeEntry> entries;
    std::ifstream input(entriesPath_);
    if (!input) {
        return entries;
    }

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }
        std::vector<std::string> parts = split(line, ';');
        if (parts.size() < 5 || parts[4] != "1") {
            continue;
        }
        entries.emplace_back(
            std::stoi(parts[0]),
            std::stoi(parts[1]),
            fromSeconds(std::stoll(parts[2])),
            fromSeconds(std::stoll(parts[3]))
        );
    }
    return entries;
}

void Storage::saveTasks(const std::vector<Task>& tasks) const {
    std::ofstream output(tasksPath_);
    if (!output) {
        throw std::runtime_error("Cannot open tasks file for writing");
    }
    for (const Task& task : tasks) {
        output << task.id() << ';'
               << task.name() << ';'
               << task.description() << ';'
               << (task.isActive() ? "1" : "0") << '\n';
    }
}

void Storage::saveEntries(const std::vector<TimeEntry>& entries) const {
    std::ofstream output(entriesPath_);
    if (!output) {
        throw std::runtime_error("Cannot open entries file for writing");
    }
    for (const TimeEntry& entry : entries) {
        output << entry.id() << ';'
               << entry.taskId() << ';'
               << toSeconds(entry.start()) << ';'
               << toSeconds(entry.end()) << ';'
               << (entry.isClosed() ? "1" : "0") << '\n';
    }
}

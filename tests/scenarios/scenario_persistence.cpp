#include "Storage.h"

#include <chrono>
#include <cstdio>
#include <iostream>

int main() {
    const char* tasksPath = "scenario_tasks.csv";
    const char* entriesPath = "scenario_entries.csv";

    Storage storage(tasksPath, entriesPath);
    storage.saveTasks({Task(1, "Persisted task", "Saved to file")});
    storage.saveEntries({
        TimeEntry(
            1,
            1,
            std::chrono::system_clock::time_point{} + std::chrono::minutes(10),
            std::chrono::system_clock::time_point{} + std::chrono::minutes(40)
        )
    });

    auto tasks = storage.loadTasks();
    auto entries = storage.loadEntries();

    std::remove(tasksPath);
    std::remove(entriesPath);

    std::cout << "Scenario persistence: " << tasks.size() << " task, "
              << entries.size() << " entry\n";
    return tasks.size() == 1 && entries.size() == 1 && entries.front().durationMinutes() == 30 ? 0 : 1;
}

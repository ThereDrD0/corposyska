#include "ConsoleApp.h"
#include "ReportService.h"
#include "Storage.h"
#include "TimeTrackerService.h"

#include <chrono>
#include <iostream>
#include <string>

namespace {
using Clock = std::chrono::system_clock;

void printReport(const TimeTrackerService& tracker) {
    ReportService report;
    auto totals = report.minutesByTask(tracker.tasks(), tracker.entries());
    std::cout << "Report by task:\n";
    for (const auto& item : totals) {
        std::cout << "  " << item.first << ": " << item.second << " minutes\n";
    }
}
}

int main(int argc, char* argv[]) {
    TimeTrackerService tracker;
    int taskId = tracker.createTask("Practice 3", "CMake project setup");

    if (argc > 1 && std::string(argv[1]) == "--demo") {
        auto start = Clock::time_point{} + std::chrono::minutes(30);
        auto end = Clock::time_point{} + std::chrono::minutes(95);
        tracker.addManualEntry(taskId, start, end);
        printReport(tracker);

        Storage storage("tasks.csv", "entries.csv");
        storage.saveTasks(tracker.tasks());
        storage.saveEntries(tracker.entries());
        std::cout << "Demo data saved to tasks.csv and entries.csv\n";
        return 0;
    }

    ConsoleApp app(Storage("tasks.csv", "entries.csv"), std::cin, std::cout);
    app.run();
    return 0;
}

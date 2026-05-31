#include "ReportService.h"
#include "TimeTrackerService.h"

#include <chrono>
#include <iostream>

int main() {
    TimeTrackerService tracker;
    int taskId = tracker.createTask("Scenario task", "Basic workflow");

    tracker.startTask(taskId, std::chrono::system_clock::time_point{} + std::chrono::minutes(15));
    tracker.stopCurrentTask(std::chrono::system_clock::time_point{} + std::chrono::minutes(75));

    ReportService report;
    auto totals = report.minutesByTask(tracker.tasks(), tracker.entries());

    std::cout << "Scenario basic workflow: " << totals["Scenario task"] << " minutes\n";
    return totals["Scenario task"] == 60 ? 0 : 1;
}

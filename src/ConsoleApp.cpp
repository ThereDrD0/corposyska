#include "ConsoleApp.h"

#include "DateTimeUtils.h"
#include "ReportService.h"

#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

ConsoleApp::ConsoleApp(Storage storage, std::istream& input, std::ostream& output)
    : storage_(std::move(storage)), input_(input), output_(output) {
    load();
}

void ConsoleApp::run() {
    bool running = true;
    while (running && input_) {
        printMenu();
        int choice = readInt("Select action: ");
        try {
            switch (choice) {
            case 1:
                createTask();
                break;
            case 2:
                listTasks();
                break;
            case 3:
                startTimer();
                break;
            case 4:
                stopTimer();
                break;
            case 5:
                addManualEntry();
                break;
            case 6:
                printJournal();
                break;
            case 7:
                printReportByTask();
                break;
            case 8:
                printReportByDay();
                break;
            case 9:
                save();
                output_ << "Data saved.\n";
                break;
            case 0:
                save();
                running = false;
                break;
            default:
                output_ << "Unknown action.\n";
                break;
            }
        } catch (const std::exception& error) {
            output_ << "Error: " << error.what() << "\n";
        }
    }
}

void ConsoleApp::load() {
    for (const Task& task : storage_.loadTasks()) {
        tracker_.addTask(task);
    }
    for (const TimeEntry& entry : storage_.loadEntries()) {
        tracker_.addEntry(entry);
    }
}

void ConsoleApp::save() {
    storage_.saveTasks(tracker_.tasks());
    storage_.saveEntries(tracker_.entries());
}

void ConsoleApp::printMenu() {
    output_ << "\nTimeTracker\n"
            << "1. Create task\n"
            << "2. List tasks\n"
            << "3. Start timer\n"
            << "4. Stop timer\n"
            << "5. Add manual entry\n"
            << "6. Show journal\n"
            << "7. Report by task\n"
            << "8. Report by day\n"
            << "9. Save\n"
            << "0. Exit\n";
}

void ConsoleApp::createTask() {
    std::string name = readLine("Task name: ");
    std::string description = readLine("Description: ");
    int id = tracker_.createTask(name, description);
    output_ << "Created task #" << id << ".\n";
}

void ConsoleApp::listTasks() {
    output_ << "Tasks:\n";
    for (const Task& task : tracker_.tasks()) {
        output_ << "  #" << task.id() << " " << task.name()
                << (task.isActive() ? " [active]" : " [archived]") << "\n";
    }
}

void ConsoleApp::startTimer() {
    int taskId = readInt("Task id: ");
    tracker_.startTask(taskId);
    output_ << "Timer started for task #" << taskId << ".\n";
}

void ConsoleApp::stopTimer() {
    TimeEntry entry = tracker_.stopCurrentTask();
    output_ << "Timer stopped. Duration: " << entry.durationMinutes() << " minutes.\n";
}

void ConsoleApp::addManualEntry() {
    int taskId = readInt("Task id: ");
    std::string start = readLine("Start (YYYY-MM-DD HH:MM): ");
    std::string end = readLine("End (YYYY-MM-DD HH:MM): ");
    tracker_.addManualEntry(taskId, DateTimeUtils::parseDateTime(start), DateTimeUtils::parseDateTime(end));
    output_ << "Manual entry added.\n";
}

void ConsoleApp::printJournal() {
    output_ << "Journal:\n";
    for (const TimeEntry& entry : tracker_.entries()) {
        output_ << "  #" << entry.id()
                << " task #" << entry.taskId()
                << " " << DateTimeUtils::formatDateTime(entry.start());
        if (entry.isClosed()) {
            output_ << " - " << DateTimeUtils::formatDateTime(entry.end())
                    << ", " << entry.durationMinutes() << " minutes";
        } else {
            output_ << " - running";
        }
        output_ << "\n";
    }
}

void ConsoleApp::printReportByTask() {
    ReportService report;
    output_ << "Report by task:\n";
    for (const auto& item : report.minutesByTask(tracker_.tasks(), tracker_.entries())) {
        output_ << "  " << item.first << ": " << item.second << " minutes\n";
    }
}

void ConsoleApp::printReportByDay() {
    ReportService report;
    output_ << "Report by day:\n";
    for (const auto& item : report.minutesByDay(tracker_.entries())) {
        output_ << "  " << item.first << ": " << item.second << " minutes\n";
    }
}

std::string ConsoleApp::readLine(const std::string& prompt) {
    output_ << prompt;
    std::string value;
    std::getline(input_, value);
    return value;
}

int ConsoleApp::readInt(const std::string& prompt) {
    std::string value = readLine(prompt);
    std::istringstream parser(value);
    int result = 0;
    parser >> result;
    if (parser.fail()) {
        throw std::invalid_argument("Expected integer value");
    }
    return result;
}

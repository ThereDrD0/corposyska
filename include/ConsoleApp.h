#pragma once

#include "Storage.h"
#include "TimeTrackerService.h"

#include <iosfwd>
#include <string>

class ConsoleApp {
public:
    ConsoleApp(Storage storage, std::istream& input, std::ostream& output);

    void run();

private:
    void load();
    void save();
    void printMenu();
    void createTask();
    void listTasks();
    void startTimer();
    void stopTimer();
    void addManualEntry();
    void printJournal();
    void printReportByTask();
    void printReportByDay();

    std::string readLine(const std::string& prompt);
    int readInt(const std::string& prompt);

    Storage storage_;
    TimeTrackerService tracker_;
    std::istream& input_;
    std::ostream& output_;
};

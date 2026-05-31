#include "ConsoleApp.h"
#include "Storage.h"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

int main() {
    const char* tasksPath = "scenario_console_tasks.csv";
    const char* entriesPath = "scenario_console_entries.csv";
    std::remove(tasksPath);
    std::remove(entriesPath);

    std::istringstream input(
        "1\n"
        "Console scenario\n"
        "Scripted user flow\n"
        "5\n"
        "1\n"
        "1970-01-01 10:00\n"
        "1970-01-01 11:15\n"
        "7\n"
        "0\n"
    );
    std::ostringstream output;

    ConsoleApp app(Storage(tasksPath, entriesPath), input, output);
    app.run();

    std::remove(tasksPath);
    std::remove(entriesPath);

    const std::string text = output.str();
    std::cout << "Scenario console workflow completed\n";
    return text.find("Console scenario: 75 minutes") == std::string::npos ? 1 : 0;
}

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "ConsoleApp.h"
#include "DateTimeUtils.h"
#include "ReportService.h"
#include "Storage.h"
#include "Task.h"
#include "TimeEntry.h"
#include "TimeTrackerService.h"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <sstream>
#include <stdexcept>

using Clock = std::chrono::system_clock;

namespace {
TimeEntry::TimePoint atMinutes(long long minutes) {
    return Clock::time_point{} + std::chrono::minutes(minutes);
}

void removeIfExists(const std::filesystem::path& path) {
    std::remove(path.string().c_str());
}
}

TEST_CASE("Task validates constructor input and stores fields", "[Task]") {
    Task task(1, "Учеба", "Практическая работа");

    REQUIRE(task.id() == 1);
    REQUIRE(task.name() == "Учеба");
    REQUIRE(task.description() == "Практическая работа");
    REQUIRE(task.isActive());
    REQUIRE_THROWS_AS(Task(0, "Bad"), std::invalid_argument);
    REQUIRE_THROWS_AS(Task(1, ""), std::invalid_argument);
}

TEST_CASE("Task supports rename, description update and archive", "[Task]") {
    Task task(2, "Старое название");

    task.rename("Новое название");
    task.setDescription("Новое описание");
    task.markArchived();

    REQUIRE(task.name() == "Новое название");
    REQUIRE(task.description() == "Новое описание");
    REQUIRE_FALSE(task.isActive());
    REQUIRE_THROWS_AS(task.rename(""), std::invalid_argument);
}

TEST_CASE("TimeEntry calculates duration and validates time interval", "[TimeEntry]") {
    TimeEntry opened(1, 7, atMinutes(10));
    REQUIRE_FALSE(opened.isClosed());
    REQUIRE(opened.durationMinutes() == 0);

    opened.close(atMinutes(75));
    REQUIRE(opened.isClosed());
    REQUIRE(opened.durationMinutes() == 65);
    REQUIRE(opened.start() == atMinutes(10));
    REQUIRE(opened.end() == atMinutes(75));

    REQUIRE_THROWS_AS(TimeEntry(0, 1, atMinutes(1)), std::invalid_argument);
    REQUIRE_THROWS_AS(TimeEntry(1, 0, atMinutes(1)), std::invalid_argument);
    REQUIRE_THROWS_AS(TimeEntry(2, 1, atMinutes(20), atMinutes(19)), std::invalid_argument);
}

TEST_CASE("TimeTrackerService manages tasks, timers and manual entries", "[TimeTrackerService]") {
    TimeTrackerService tracker;
    const int first = tracker.createTask("Проект", "Практика");
    const int second = tracker.createTask("Учеба");

    REQUIRE(first == 1);
    REQUIRE(second == 2);
    REQUIRE(tracker.tasks().size() == 2);

    tracker.startTask(first, atMinutes(100));
    REQUIRE(tracker.hasRunningTimer());
    REQUIRE_THROWS_AS(tracker.startTask(second, atMinutes(101)), std::logic_error);

    TimeEntry closed = tracker.stopCurrentTask(atMinutes(145));
    REQUIRE_FALSE(tracker.hasRunningTimer());
    REQUIRE(closed.durationMinutes() == 45);
    REQUIRE(tracker.entries().front().durationMinutes() == 45);

    tracker.addManualEntry(second, atMinutes(200), atMinutes(260));
    REQUIRE(tracker.entries().size() == 2);
    REQUIRE_THROWS_AS(tracker.addManualEntry(second, atMinutes(300), atMinutes(299)), std::invalid_argument);
    REQUIRE_THROWS_AS(tracker.startTask(404, atMinutes(1)), std::out_of_range);
}

TEST_CASE("TimeTrackerService renames and archives tasks", "[TimeTrackerService]") {
    TimeTrackerService tracker;
    int taskId = tracker.createTask("Черновик");

    tracker.renameTask(taskId, "Финальное название");
    REQUIRE(tracker.tasks().front().name() == "Финальное название");

    tracker.archiveTask(taskId);
    REQUIRE_FALSE(tracker.tasks().front().isActive());
    REQUIRE_THROWS_AS(tracker.startTask(taskId, atMinutes(1)), std::logic_error);
    REQUIRE_THROWS_AS(tracker.archiveTask(999), std::out_of_range);
}

TEST_CASE("ReportService builds reports by task and by day", "[ReportService]") {
    std::vector<Task> tasks = {
        Task(1, "Проект"),
        Task(2, "Учеба")
    };
    std::vector<TimeEntry> entries = {
        TimeEntry(1, 1, atMinutes(60), atMinutes(120)),
        TimeEntry(2, 1, atMinutes(24 * 60 + 10), atMinutes(24 * 60 + 40)),
        TimeEntry(3, 2, atMinutes(24 * 60 + 50), atMinutes(24 * 60 + 80)),
        TimeEntry(4, 404, atMinutes(10), atMinutes(20)),
        TimeEntry(5, 1, atMinutes(500))
    };

    ReportService report;
    auto byTask = report.minutesByTask(tasks, entries);
    auto byDay = report.minutesByDay(entries);

    REQUIRE(byTask["Проект"] == 90);
    REQUIRE(byTask["Учеба"] == 30);
    REQUIRE(byTask["Unknown task"] == 10);
    REQUIRE(byTask.count("Not exists") == 0);
    REQUIRE(byDay["1970-01-01"] == 70);
    REQUIRE(byDay["1970-01-02"] == 60);
}

TEST_CASE("DateTimeUtils parses and formats dates", "[DateTimeUtils]") {
    auto value = DateTimeUtils::parseDateTime("1970-01-02 03:04");

    REQUIRE(DateTimeUtils::formatDate(value) == "1970-01-02");
    REQUIRE(DateTimeUtils::formatDateTime(value) == "1970-01-02 03:04");
    REQUIRE_THROWS_AS(DateTimeUtils::parseDateTime("1970/01/02 03:04"), std::invalid_argument);
    REQUIRE_THROWS_AS(DateTimeUtils::parseDateTime("not a date"), std::invalid_argument);
}

TEST_CASE("Storage saves and loads tasks and entries", "[Storage]") {
    const std::filesystem::path tasksPath = "unit_tasks.csv";
    const std::filesystem::path entriesPath = "unit_entries.csv";
    removeIfExists(tasksPath);
    removeIfExists(entriesPath);

    Storage missing(tasksPath.string(), entriesPath.string());
    REQUIRE(missing.loadTasks().empty());
    REQUIRE(missing.loadEntries().empty());

    Task archived(2, "Архив", "Неактивная задача");
    archived.markArchived();
    std::vector<Task> tasks = {
        Task(1, "Проект", "Основная задача"),
        archived
    };
    std::vector<TimeEntry> entries = {
        TimeEntry(1, 1, atMinutes(10), atMinutes(30)),
        TimeEntry(2, 2, atMinutes(50), atMinutes(95))
    };

    Storage storage(tasksPath.string(), entriesPath.string());
    storage.saveTasks(tasks);
    storage.saveEntries(entries);

    auto loadedTasks = storage.loadTasks();
    auto loadedEntries = storage.loadEntries();

    REQUIRE(loadedTasks.size() == 2);
    REQUIRE(loadedTasks[0].name() == "Проект");
    REQUIRE_FALSE(loadedTasks[1].isActive());
    REQUIRE(loadedEntries.size() == 2);
    REQUIRE(loadedEntries[0].durationMinutes() == 20);
    REQUIRE(loadedEntries[1].durationMinutes() == 45);

    removeIfExists(tasksPath);
    removeIfExists(entriesPath);
}

TEST_CASE("ConsoleApp executes a full scripted user workflow", "[ConsoleApp]") {
    const std::filesystem::path tasksPath = "console_tasks.csv";
    const std::filesystem::path entriesPath = "console_entries.csv";
    removeIfExists(tasksPath);
    removeIfExists(entriesPath);

    std::istringstream input(
        "1\n"
        "Проект\n"
        "Подготовка практической работы\n"
        "5\n"
        "1\n"
        "1970-01-01 01:00\n"
        "1970-01-01 02:30\n"
        "6\n"
        "7\n"
        "8\n"
        "0\n"
    );
    std::ostringstream output;

    ConsoleApp app(Storage(tasksPath.string(), entriesPath.string()), input, output);
    app.run();

    const std::string rendered = output.str();
    REQUIRE(rendered.find("Manual entry added.") != std::string::npos);
    REQUIRE(rendered.find("Journal:") != std::string::npos);
    REQUIRE(rendered.find("Проект: 90 minutes") != std::string::npos);
    REQUIRE(rendered.find("1970-01-01: 90 minutes") != std::string::npos);
    REQUIRE(Storage(tasksPath.string(), entriesPath.string()).loadTasks().size() == 1);
    REQUIRE(Storage(tasksPath.string(), entriesPath.string()).loadEntries().size() == 1);

    removeIfExists(tasksPath);
    removeIfExists(entriesPath);
}

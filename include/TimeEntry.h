#pragma once

#include <chrono>

class TimeEntry {
public:
    using TimePoint = std::chrono::system_clock::time_point;

    TimeEntry(int id, int taskId, TimePoint start);
    TimeEntry(int id, int taskId, TimePoint start, TimePoint end);

    int id() const;
    int taskId() const;
    TimePoint start() const;
    TimePoint end() const;
    bool isClosed() const;

    void close(TimePoint end);
    long long durationMinutes() const;

private:
    int id_;
    int taskId_;
    TimePoint start_;
    TimePoint end_;
    bool closed_;
};

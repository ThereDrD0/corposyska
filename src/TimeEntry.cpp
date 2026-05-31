#include "TimeEntry.h"

#include <stdexcept>

TimeEntry::TimeEntry(int id, int taskId, TimePoint start)
    : id_(id), taskId_(taskId), start_(start), end_(start), closed_(false) {
    if (id_ <= 0 || taskId_ <= 0) {
        throw std::invalid_argument("Entry id and task id must be positive");
    }
}

TimeEntry::TimeEntry(int id, int taskId, TimePoint start, TimePoint end)
    : TimeEntry(id, taskId, start) {
    close(end);
}

int TimeEntry::id() const {
    return id_;
}

int TimeEntry::taskId() const {
    return taskId_;
}

TimeEntry::TimePoint TimeEntry::start() const {
    return start_;
}

TimeEntry::TimePoint TimeEntry::end() const {
    return end_;
}

bool TimeEntry::isClosed() const {
    return closed_;
}

void TimeEntry::close(TimePoint end) {
    if (end < start_) {
        throw std::invalid_argument("Entry end must not be earlier than start");
    }
    end_ = end;
    closed_ = true;
}

long long TimeEntry::durationMinutes() const {
    if (!closed_) {
        return 0;
    }
    return std::chrono::duration_cast<std::chrono::minutes>(end_ - start_).count();
}

#include "Task.h"

#include <stdexcept>
#include <utility>

Task::Task(int id, std::string name, std::string description, bool active)
    : id_(id), name_(std::move(name)), description_(std::move(description)), active_(active) {
    if (id_ <= 0) {
        throw std::invalid_argument("Task id must be positive");
    }
    if (name_.empty()) {
        throw std::invalid_argument("Task name must not be empty");
    }
}

int Task::id() const {
    return id_;
}

const std::string& Task::name() const {
    return name_;
}

const std::string& Task::description() const {
    return description_;
}

bool Task::isActive() const {
    return active_;
}

void Task::rename(const std::string& newName) {
    if (newName.empty()) {
        throw std::invalid_argument("Task name must not be empty");
    }
    name_ = newName;
}

void Task::setDescription(const std::string& description) {
    description_ = description;
}

void Task::markArchived() {
    active_ = false;
}

#pragma once

#include <string>

class Task {
public:
    Task(int id, std::string name, std::string description = "", bool active = true);

    int id() const;
    const std::string& name() const;
    const std::string& description() const;
    bool isActive() const;

    void rename(const std::string& newName);
    void setDescription(const std::string& description);
    void markArchived();

private:
    int id_;
    std::string name_;
    std::string description_;
    bool active_;
};

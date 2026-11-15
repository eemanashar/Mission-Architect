#pragma once

#include <string>
#include <vector>

class Mission {
public:
    Mission(std::string name, std::string destination, std::string scienceGoals,
            double budgetMillion, std::string difficulty, std::string briefing);

    const std::string& name() const { return name_; }
    const std::string& destination() const { return destination_; }
    const std::string& scienceGoals() const { return scienceGoals_; }
    double budgetMillion() const { return budgetMillion_; }
    const std::string& difficulty() const { return difficulty_; }
    const std::string& briefing() const { return briefing_; }

private:
    std::string name_;
    std::string destination_;
    std::string scienceGoals_;
    double budgetMillion_;
    std::string difficulty_;
    std::string briefing_;
};

std::vector<Mission> availableMissions();

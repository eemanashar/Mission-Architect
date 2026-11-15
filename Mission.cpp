#include "Mission.h"

Mission::Mission(std::string name, std::string destination, std::string scienceGoals,
                 double budgetMillion, std::string difficulty, std::string briefing)
    : name_(std::move(name))
    , destination_(std::move(destination))
    , scienceGoals_(std::move(scienceGoals))
    , budgetMillion_(budgetMillion)
    , difficulty_(std::move(difficulty))
    , briefing_(std::move(briefing)) {}

std::vector<Mission> availableMissions() {
    return {
        Mission(
            "Moon Orbiter",
            "The Moon",
            "Map craters and measure the radiation environment.",
            400,
            "Intermediate",
            "Orbit the Moon, photograph the surface, and send the data home. "
            "Solar power works, but a small dish will choke on a busy instrument set.")
    };
}

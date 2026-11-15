#pragma once

#include "Mission.h"

#include <string>
#include <vector>

enum class Body { Moon, Mars, Asteroid, EarthOrbit };

struct MissionProfile {
    Body body = Body::Moon;
    bool flyby = false;               
    double scienceTarget = 40.0;      
    double requiredPropFraction = 0.10;
    double flareChance = 0.40;
    double dropoutChance = 0.30;
    double linkScale = 1.0;           
};

MissionProfile profileFor(const std::string& missionName);

std::vector<Mission> allMissions(); 

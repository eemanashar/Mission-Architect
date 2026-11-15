#include "MissionProfile.h"

MissionProfile profileFor(const std::string& name) {
    MissionProfile p;  

    if (name == "Mars Flyby") {
        p.body = Body::Mars;
        p.flyby = true;
        p.scienceTarget = 52;
        p.requiredPropFraction = 0.08;
        p.flareChance = 0.55;
        p.dropoutChance = 0.30;
        p.linkScale = 0.6;
    }
    else if (name == "Earth Observation Satellite") {
        p.body = Body::EarthOrbit;
        p.scienceTarget = 30;
        p.requiredPropFraction = 0.04;
        p.flareChance = 0.25;
        p.dropoutChance = 0.20;
        p.linkScale = 1.0;
    }
    else if (name == "Asteroid Sample Return") {
        p.body = Body::Asteroid;
        p.scienceTarget = 58;
        p.requiredPropFraction = 0.16;
        p.flareChance = 0.45;
        p.dropoutChance = 0.30;
        p.linkScale = 0.7;
    }
    return p;
}

std::vector<Mission> allMissions() {
    std::vector<Mission> list = availableMissions();

    list.push_back(Mission(
        "Mars Flyby", "Mars",
        "Photograph the surface and analyze the atmosphere during a close pass.",
        600.0, "Hard",
        "Mars is far away, so your downlink runs at only 60% of its rated speed and solar flares "
        "are common on the long cruise. You only need a small course-correction burn, not a full capture."));

    list.push_back(Mission(
        "Earth Observation Satellite", "Low Earth orbit",
        "Image land and weather from orbit and stream the data to the ground.",
        300.0, "Easy",
        "A short trip with few hazards, but imaging instruments produce a lot of data. "
        "Make sure your comms system can keep up."));

    list.push_back(Mission(
        "Asteroid Sample Return", "Near-Earth asteroid",
        "Rendezvous with an asteroid, study its surface and return the data.",
        550.0, "Very Hard",
        "Matching orbits with a tiny asteroid takes a big propulsion system (about 16% of spacecraft mass). "
        "The link is weak at this distance, about 70% of its rated speed."));

    return list;
}
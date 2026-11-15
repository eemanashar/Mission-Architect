#include "MissionProfile.h"

MissionProfile profileFor(const std::string& name) {
    MissionProfile p;  // defaults, then override per mission

    if (name == "Moon Orbiter") {            // modeled on LRO
        p.body = Body::Moon;
        p.scienceTarget = 50;
        p.requiredPropFraction = 0.10;
        p.flareChance = 0.40;
        p.dropoutChance = 0.30;
        p.linkScale = 1.0;
    }
    else if (name == "Mars Flyby") {       // modeled on MRO
        p.body = Body::Mars;
        p.flyby = true;
        p.scienceTarget = 60;
        p.requiredPropFraction = 0.08;
        p.flareChance = 0.55;
        p.dropoutChance = 0.30;
        p.linkScale = 0.30;
    }
    else if (name == "Earth Observation Satellite") {  // modeled on Landsat 9
        p.body = Body::EarthOrbit;
        p.scienceTarget = 35;
        p.requiredPropFraction = 0.04;
        p.flareChance = 0.25;
        p.dropoutChance = 0.20;
        p.linkScale = 1.0;
    }
    else if (name == "Asteroid Sample Return") {       // modeled on OSIRIS-REx
        p.body = Body::Asteroid;
        p.scienceTarget = 65;
        p.requiredPropFraction = 0.16;
        p.flareChance = 0.45;
        p.dropoutChance = 0.30;
        p.linkScale = 0.45;
    }
    return p;
}

std::vector<Mission> allMissions() {
    std::vector<Mission> list;

    list.push_back(Mission(
        "Moon Orbiter", "The Moon",
        "Map craters and measure the radiation environment.",
        500.0, "Intermediate",
        "Orbit the Moon and send the data home. Modeled on NASA's LRO (about 1,900 kg, roughly $500M)."));

    list.push_back(Mission(
        "Mars Flyby", "Mars",
        "Photograph the surface and study the atmosphere in one close pass.",
        540.0, "Hard",
        "Modeled on NASA's MRO. The link to Mars is weak and flares are common. A small burn is enough."));

    list.push_back(Mission(
        "Earth Observation Satellite", "Low Earth orbit",
        "Image land and weather from orbit and stream it to the ground.",
        400.0, "Easy",
        "Modeled on Landsat 9. A short trip, but imagers produce a lot of data."));

    list.push_back(Mission(
        "Asteroid Sample Return", "Near-Earth asteroid",
        "Rendezvous with an asteroid and study its surface.",
        600.0, "Very Hard",
        "Modeled on OSIRIS-REx. It needs big propulsion (about 16% of mass) and the link is weak."));

    return list;
}
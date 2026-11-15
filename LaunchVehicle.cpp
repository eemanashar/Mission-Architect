#include "LaunchVehicle.h"

#include <sstream>

LaunchVehicle::LaunchVehicle(std::string name, std::string blurb, double capacityKg,
                             double costMillion, double reliability)
    : name_(std::move(name))
    , blurb_(std::move(blurb))
    , capacityKg_(capacityKg)
    , costMillion_(costMillion)
    , reliability_(reliability) {}

std::string LaunchVehicle::statsLine() const {
    std::ostringstream line;
    line << "$" << static_cast<int>(costMillion_) << "M   "
         << static_cast<int>(capacityKg_) << " kg capacity   "
         << static_cast<int>(reliability_ * 100) << "% reliable";
    return line.str();
}

std::vector<LaunchVehicle> availableLaunchers() {
    return {
        LaunchVehicle(
            "Spark",
            "Cheap rideshare. Fine for a light bus and one small instrument.",
            250, 30, 0.90),
        LaunchVehicle(
            "Apex 2",
            "The practical choice for a lunar orbiter with a real payload.",
            500, 75, 0.96),
        LaunchVehicle(
            "Heavy Centaur",
            "Lots of spare mass. The price eats a deep-space budget quickly.",
            1200, 160, 0.98)
    };
}

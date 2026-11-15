#include "Catalog.h"

#include "Systems.h"

#include <vector>

namespace {
const std::vector<PartSpec>& table(PartCategory category) {
    static const std::vector<PartSpec> buses = {
        {"bus.light", "Light Bus",
         "Small and cheap. Little room and a modest power draw.",
         35, 150, 30, 0, 0, 0, 56},
        {"bus.standard", "Standard Bus",
         "A balanced lunar bus with room for two instruments.",
         60, 240, 45, 0, 0, 0, 72},
        {"bus.deep", "Deep-Space Bus",
         "Sturdy and power-hungry. Built for a long cruise.",
         95, 360, 60, 0, 0, 0, 96}
    };

    static const std::vector<PartSpec> power = {
        {"power.compact", "Compact Solar Array",
         "Enough sun-power for a camera and a small dish.",
         20, 35, 0, 160, 0, 0, 70},
        {"power.wide", "Wide Solar Array",
         "Large wings. Covers radar, but they add mass.",
         48, 80, 0, 420, 0, 0, 130},
        {"power.battery", "Solar plus Battery",
         "Mid-size wings plus extra battery for lunar eclipse.",
         36, 55, 0, 220, 0, 0, 96},
        {"power.rtg", "Radioisotope RTG",
         "Power in shadow or deep space. Very expensive.",
         140, 48, 0, 260, 0, 0, 0}
    };

    static const std::vector<PartSpec> comms = {
        {"comms.low", "Low-Gain Antenna",
         "A stub antenna. Only a trickle of data gets home.",
         6, 6, 10, 0, 0.5, 0, 8},
        {"comms.mid", "Medium Dish",
         "Carries one busy instrument. Two will overflow it.",
         22, 18, 30, 0, 8, 0, 16},
        {"comms.high", "High-Gain Dish",
         "Fast downlink. Heavy, thirsty, and not cheap.",
         55, 40, 70, 0, 25, 0, 26}
    };

    static const std::vector<PartSpec> propulsion = {
        {"prop.cold", "Cold-Gas Thrusters",
         "Simple and light. Little propellant for orbit insertion.",
         8, 12, 4, 0, 0, 0, 14},
        {"prop.mono", "Monopropellant",
         "Enough delta-v for lunar orbit on a normal bus.",
         24, 35, 12, 0, 0, 0, 22},
        {"prop.bi", "Bipropellant",
         "High thrust margin. The tanks are the heavy part.",
         48, 70, 20, 0, 0, 0, 32}
    };

    static const std::vector<PartSpec> instruments = {
        {"inst.camera", "Mapping Camera",
         "Images the surface. A medium dish can just keep up.",
         18, 8, 15, 0, 6, 20, 0},
        {"inst.spec", "Spectrometer",
         "Reads minerals. Pair it with the camera and the dish fills up.",
         32, 16, 30, 0, 3, 30, 0},
        {"inst.radar", "Radar Altimeter",
         "Best science, worst power and data bill.",
         50, 26, 55, 0, 10, 40, 0},
        {"inst.mag", "Magnetometer",
         "Light, cheap, and quiet on the radio.",
         14, 3, 5, 0, 0.5, 12, 0}
    };

    switch (category) {
    case PartCategory::Structure: return buses;
    case PartCategory::Power: return power;
    case PartCategory::Comms: return comms;
    case PartCategory::Propulsion: return propulsion;
    case PartCategory::Instrument: return instruments;
    }
    return buses;
}

} 

int Catalog::count(PartCategory category) {
    return static_cast<int>(table(category).size());
}

const PartSpec& Catalog::spec(PartCategory category, int index) {
    return table(category).at(static_cast<size_t>(index));
}

std::unique_ptr<Component> Catalog::make(PartCategory category, int index) {
    if (index < 0 || index >= count(category))
        return nullptr;

    const PartSpec& row = spec(category, index);
    switch (category) {
    case PartCategory::Structure: return std::make_unique<StructureBus>(row);
    case PartCategory::Power: return std::make_unique<PowerSystem>(row);
    case PartCategory::Comms: return std::make_unique<CommsSystem>(row);
    case PartCategory::Propulsion: return std::make_unique<PropulsionSystem>(row);
    case PartCategory::Instrument: return std::make_unique<Instrument>(row);
    }
    return nullptr;
}

#include "Catalog.h"

#include "Systems.h"

#include <cstddef>
#include <vector>

namespace {

    // Numbers are scaled from the original stage-1 values so a typical build is the size of
    // the NASA reference spacecraft: mass x5, power x5, data x4, cost x1.6.
    // Row order: id, name, blurb, cost ($M), mass (kg), power draw (W), power generated (W),
    //            data Mbps (comms = downlink capacity, instruments = data produced), science, visual.
    const std::vector<PartSpec>& table(PartCategory category) {
        static const std::vector<PartSpec> buses = {
            {"bus.light", "Light Bus",
             "Small and cheap. Little room and a modest power draw.",
             56, 750, 150, 0, 0, 0, 56},
            {"bus.standard", "Standard Bus",
             "A balanced lunar bus with room for two instruments.",
             96, 1200, 225, 0, 0, 0, 72},
            {"bus.deep", "Deep-Space Bus",
             "Sturdy and power-hungry. Built for a long cruise.",
             152, 1800, 300, 0, 0, 0, 96}
        };

        static const std::vector<PartSpec> power = {
            {"power.compact", "Compact Solar Array",
             "Enough sun-power for a camera and a small dish.",
             32, 175, 0, 800, 0, 0, 70},
            {"power.wide", "Wide Solar Array",
             "Large wings. Covers radar, but they add mass.",
             77, 400, 0, 2100, 0, 0, 130},
            {"power.battery", "Solar plus Battery",
             "Mid-size wings plus extra battery for lunar eclipse.",
             58, 275, 0, 1100, 0, 0, 96},
            {"power.rtg", "Radioisotope RTG",
             "Power in shadow or deep space. Very expensive.",
             224, 240, 0, 1300, 0, 0, 0}
        };

        static const std::vector<PartSpec> comms = {
            {"comms.low", "Low-Gain Antenna",
             "A stub antenna. Only a trickle of data gets home.",
             10, 30, 50, 0, 2, 0, 8},
            {"comms.mid", "Medium Dish",
             "Carries one busy instrument. Two will overflow it.",
             35, 90, 150, 0, 32, 0, 16},
            {"comms.high", "High-Gain Dish",
             "Fast downlink (100 Mbps, like LRO). Heavy, thirsty, and not cheap.",
             88, 200, 350, 0, 100, 0, 26}
        };

        static const std::vector<PartSpec> propulsion = {
            {"prop.cold", "Cold-Gas Thrusters",
             "Simple and light. Little propellant for orbit insertion.",
             13, 60, 20, 0, 0, 0, 14},
            {"prop.mono", "Monopropellant",
             "Enough delta-v for lunar orbit on a normal bus.",
             38, 175, 60, 0, 0, 0, 22},
            {"prop.bi", "Bipropellant",
             "High thrust margin. The tanks are the heavy part.",
             77, 350, 100, 0, 0, 0, 32}
        };

        static const std::vector<PartSpec> instruments = {
            {"inst.camera", "Mapping Camera",
             "Images the surface. A medium dish can just keep up.",
             29, 40, 75, 0, 24, 20, 0},
            {"inst.spec", "Spectrometer",
             "Reads minerals. Pair it with the camera and the dish fills up.",
             51, 80, 150, 0, 12, 30, 0},
            {"inst.radar", "Radar Altimeter",
             "Best science, worst power and data bill.",
             80, 130, 275, 0, 40, 40, 0},
            {"inst.mag", "Magnetometer",
             "Light, cheap, and quiet on the radio.",
             22, 15, 25, 0, 2, 12, 0}
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

}  // namespace

int Catalog::count(PartCategory category) {
    return static_cast<int>(table(category).size());
}

const PartSpec& Catalog::spec(PartCategory category, int index) {
    return table(category).at(static_cast<std::size_t>(index));
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
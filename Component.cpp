#include "Component.h"

#include <iomanip>
#include <sstream>

Component::Component(PartSpec spec) : spec_(std::move(spec)) {}

double Component::generatedPowerW() const {
    return spec_.powerGenW;
}

double Component::consumedPowerW() const {
    return spec_.powerDrawW;
}

double Component::downlinkMbps() const {
    return 0;
}

double Component::producedDataMbps() const {
    return 0;
}

int Component::scienceValue() const {
    return 0;
}

std::string Component::statsLine() const {
    std::ostringstream line;
    line << "$" << static_cast<int>(spec_.costMillion) << "M   "
         << static_cast<int>(spec_.massKg) << " kg   ";

    if (spec_.powerGenW > 0)
        line << "+" << static_cast<int>(spec_.powerGenW) << " W   ";
    if (spec_.powerDrawW > 0)
        line << "-" << static_cast<int>(spec_.powerDrawW) << " W   ";
    if (spec_.dataMbps > 0) {
        std::ostringstream data;
        data << std::fixed << std::setprecision(spec_.dataMbps < 1.0 ? 1 : 0) << spec_.dataMbps;
        line << data.str() << " Mbps   ";
    }
    if (spec_.science > 0)
        line << "science +" << spec_.science;

    return line.str();
}

const char* categoryName(PartCategory category) {
    switch (category) {
    case PartCategory::Structure: return "Bus";
    case PartCategory::Power: return "Power";
    case PartCategory::Comms: return "Comms";
    case PartCategory::Propulsion: return "Propulsion";
    case PartCategory::Instrument: return "Instrument";
    }
    return "Part";
}

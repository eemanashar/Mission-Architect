#include "Systems.h"

#include <sstream>

StructureBus::StructureBus(PartSpec spec) : Component(std::move(spec)) {}

PartCategory StructureBus::category() const {
    return PartCategory::Structure;
}

std::unique_ptr<Component> StructureBus::clone() const {
    return std::make_unique<StructureBus>(*this);
}

std::string StructureBus::detailLine() const {
    return spec_.blurb + " " + statsLine();
}

PowerSystem::PowerSystem(PartSpec spec) : Component(std::move(spec)) {}

PartCategory PowerSystem::category() const {
    return PartCategory::Power;
}

std::unique_ptr<Component> PowerSystem::clone() const {
    return std::make_unique<PowerSystem>(*this);
}

std::string PowerSystem::detailLine() const {
    std::ostringstream line;
    line << spec_.blurb << " Output " << static_cast<int>(generatedPowerW()) << " W. " << statsLine();
    return line.str();
}

bool PowerSystem::isRadioisotope() const {
    return spec_.visual <= 0.f && spec_.powerGenW > 0;
}

CommsSystem::CommsSystem(PartSpec spec) : Component(std::move(spec)) {}

PartCategory CommsSystem::category() const {
    return PartCategory::Comms;
}

std::unique_ptr<Component> CommsSystem::clone() const {
    return std::make_unique<CommsSystem>(*this);
}

std::string CommsSystem::detailLine() const {
    std::ostringstream line;
    line << spec_.blurb << " Downlink " << downlinkMbps() << " Mbps. " << statsLine();
    return line.str();
}

double CommsSystem::downlinkMbps() const {
    return spec_.dataMbps;
}

PropulsionSystem::PropulsionSystem(PartSpec spec) : Component(std::move(spec)) {}

PartCategory PropulsionSystem::category() const {
    return PartCategory::Propulsion;
}

std::unique_ptr<Component> PropulsionSystem::clone() const {
    return std::make_unique<PropulsionSystem>(*this);
}

std::string PropulsionSystem::detailLine() const {
    return spec_.blurb + " " + statsLine();
}

Instrument::Instrument(PartSpec spec) : Component(std::move(spec)) {}

PartCategory Instrument::category() const {
    return PartCategory::Instrument;
}

std::unique_ptr<Component> Instrument::clone() const {
    return std::make_unique<Instrument>(*this);
}

std::string Instrument::detailLine() const {
    std::ostringstream line;
    line << spec_.blurb << " Science +" << scienceValue() << ". " << statsLine();
    return line.str();
}

double Instrument::producedDataMbps() const {
    return spec_.dataMbps;
}

int Instrument::scienceValue() const {
    return spec_.science;
}

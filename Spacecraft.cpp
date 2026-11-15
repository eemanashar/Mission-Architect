#include "Spacecraft.h"

#include "Catalog.h"

namespace {

void addPart(const Component* part, ResourceReport& report) {
    if (!part)
        return;

    const PartSpec& spec = part->spec();
    report.partsCostMillion += spec.costMillion;
    report.massKg += spec.massKg;
    report.powerDrawW += part->consumedPowerW();
    report.powerGenW += part->generatedPowerW();
    report.dataOutMbps += part->producedDataMbps();
    report.dataCapMbps += part->downlinkMbps();
    report.science += part->scienceValue();
}

}

void Spacecraft::select(PartCategory category, int catalogIndex) {
    if (category == PartCategory::Instrument) {
        toggleInstrument(catalogIndex);
        return;
    }

    std::unique_ptr<Component> part = Catalog::make(category, catalogIndex);
    if (!part)
        return;

    switch (category) {
    case PartCategory::Structure:
        structure_ = std::move(part);
        structureIndex_ = catalogIndex;
        break;
    case PartCategory::Power:
        power_ = std::move(part);
        powerIndex_ = catalogIndex;
        break;
    case PartCategory::Comms:
        comms_ = std::move(part);
        commsIndex_ = catalogIndex;
        break;
    case PartCategory::Propulsion:
        propulsion_ = std::move(part);
        propulsionIndex_ = catalogIndex;
        break;
    case PartCategory::Instrument:
        break;
    }
}

void Spacecraft::toggleInstrument(int catalogIndex) {
    std::unique_ptr<Component> part = Catalog::make(PartCategory::Instrument, catalogIndex);
    if (!part)
        return;

    const std::string id = part->spec().id;
    for (auto it = instruments_.begin(); it != instruments_.end(); ++it) {
        if ((*it)->spec().id == id) {
            instruments_.erase(it);
            return;
        }
    }
    instruments_.push_back(std::move(part));
}

void Spacecraft::clear() {
    structure_.reset();
    power_.reset();
    comms_.reset();
    propulsion_.reset();
    instruments_.clear();
    structureIndex_ = powerIndex_ = commsIndex_ = propulsionIndex_ = -1;
}

bool Spacecraft::isSelected(PartCategory category, int catalogIndex) const {
    switch (category) {
    case PartCategory::Structure: return structureIndex_ == catalogIndex;
    case PartCategory::Power: return powerIndex_ == catalogIndex;
    case PartCategory::Comms: return commsIndex_ == catalogIndex;
    case PartCategory::Propulsion: return propulsionIndex_ == catalogIndex;
    case PartCategory::Instrument:
        if (catalogIndex < 0 || catalogIndex >= Catalog::count(PartCategory::Instrument))
            return false;
        {
            const std::string& id = Catalog::spec(PartCategory::Instrument, catalogIndex).id;
            for (const auto& owned : instruments_) {
                if (owned->spec().id == id)
                    return true;
            }
        }
        return false;
    }
    return false;
}

ResourceReport Spacecraft::evaluate(double budgetMillion, const LaunchVehicle* launcher) const {
    ResourceReport report;
    report.budgetMillion = budgetMillion;

    addPart(structure_.get(), report);
    addPart(power_.get(), report);
    addPart(comms_.get(), report);
    addPart(propulsion_.get(), report);
    for (const auto& instrument : instruments_)
        addPart(instrument.get(), report);

    if (launcher) {
        report.launchCostMillion = launcher->costMillion();
        report.capacityKg = launcher->capacityKg();
    }
    return report;
}

#pragma once

#include "Component.h"
#include "LaunchVehicle.h"

#include <memory>
#include <vector>

struct ResourceReport {
    double budgetMillion = 0;
    double partsCostMillion = 0;
    double launchCostMillion = 0;
    double massKg = 0;
    double capacityKg = 0;
    double powerDrawW = 0;
    double powerGenW = 0;
    double dataOutMbps = 0;
    double dataCapMbps = 0;
    int science = 0;

    double totalCost() const { return partsCostMillion + launchCostMillion; }
    bool hasLauncher() const { return capacityKg > 0; }
    bool overBudget() const { return totalCost() > budgetMillion; }
    bool overMass() const { return hasLauncher() && massKg > capacityKg; }
    bool powerShort() const { return powerGenW <= 0 || powerDrawW > powerGenW; }
    bool dataShort() const { return dataCapMbps <= 0 || dataOutMbps > dataCapMbps; }
    bool fits() const { return !overBudget() && !overMass() && !powerShort() && !dataShort() && hasLauncher(); }
};

class Spacecraft {
public:
    void select(PartCategory category, int catalogIndex);
    void toggleInstrument(int catalogIndex);
    void clear();

    bool isSelected(PartCategory category, int catalogIndex) const;

    const Component* structure() const { return structure_.get(); }
    const Component* power() const { return power_.get(); }
    const Component* comms() const { return comms_.get(); }
    const Component* propulsion() const { return propulsion_.get(); }
    const std::vector<std::unique_ptr<Component>>& instruments() const { return instruments_; }

    ResourceReport evaluate(double budgetMillion, const LaunchVehicle* launcher) const;

private:
    std::unique_ptr<Component> structure_;
    std::unique_ptr<Component> power_;
    std::unique_ptr<Component> comms_;
    std::unique_ptr<Component> propulsion_;
    std::vector<std::unique_ptr<Component>> instruments_;

    int structureIndex_ = -1;
    int powerIndex_ = -1;
    int commsIndex_ = -1;
    int propulsionIndex_ = -1;
};

#pragma once

#include "Component.h"


class StructureBus : public Component {
public:
    explicit StructureBus(PartSpec spec);
    PartCategory category() const override;
    std::unique_ptr<Component> clone() const override;
    std::string detailLine() const override;
};

class PowerSystem : public Component {
public:
    explicit PowerSystem(PartSpec spec);
    PartCategory category() const override;
    std::unique_ptr<Component> clone() const override;
    std::string detailLine() const override;
    bool isRadioisotope() const;
};

class CommsSystem : public Component {
public:
    explicit CommsSystem(PartSpec spec);
    PartCategory category() const override;
    std::unique_ptr<Component> clone() const override;
    std::string detailLine() const override;
    double downlinkMbps() const override;
};

class PropulsionSystem : public Component {
public:
    explicit PropulsionSystem(PartSpec spec);
    PartCategory category() const override;
    std::unique_ptr<Component> clone() const override;
    std::string detailLine() const override;
};

class Instrument : public Component {
public:
    explicit Instrument(PartSpec spec);
    PartCategory category() const override;
    std::unique_ptr<Component> clone() const override;
    std::string detailLine() const override;
    double producedDataMbps() const override;
    int scienceValue() const override;
};

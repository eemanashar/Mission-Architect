#pragma once

#include <memory>
#include <string>

struct PartSpec {
    std::string id;
    std::string name;
    std::string blurb;
    double costMillion = 0;
    double massKg = 0;
    double powerDrawW = 0;
    double powerGenW = 0;
    double dataMbps = 0;
    int science = 0;
    float visual = 0.f;
};

enum class PartCategory {
    Structure,
    Power,
    Comms,
    Propulsion,
    Instrument
};

class Component {
public:
    explicit Component(PartSpec spec);
    virtual ~Component() = default;

    const PartSpec& spec() const { return spec_; }
    const std::string& name() const { return spec_.name; }

    virtual PartCategory category() const = 0;
    virtual std::unique_ptr<Component> clone() const = 0;
    virtual std::string detailLine() const = 0;

    virtual double generatedPowerW() const;
    virtual double consumedPowerW() const;
    virtual double downlinkMbps() const;
    virtual double producedDataMbps() const;
    virtual int scienceValue() const;

    std::string statsLine() const;

protected:
    PartSpec spec_;
};

const char* categoryName(PartCategory category);

#pragma once

#include <string>
#include <vector>

class LaunchVehicle {
public:
    LaunchVehicle(std::string name, std::string blurb, double capacityKg,
                  double costMillion, double reliability);

    const std::string& name() const { return name_; }
    const std::string& blurb() const { return blurb_; }
    double capacityKg() const { return capacityKg_; }
    double costMillion() const { return costMillion_; }
    double reliability() const { return reliability_; }

    std::string statsLine() const;

private:
    std::string name_;
    std::string blurb_;
    double capacityKg_;
    double costMillion_;
    double reliability_;
};

std::vector<LaunchVehicle> availableLaunchers();

#pragma once

#include "Component.h"

#include <memory>

class Catalog {
public:
    static int count(PartCategory category);
    static const PartSpec& spec(PartCategory category, int index);
    static std::unique_ptr<Component> make(PartCategory category, int index);
};

#pragma once

#include "route.h"
#include <vector>

class Module {
public:
    virtual ~Module() = default;
    virtual const std::vector<Route> getRoutes() const = 0;
};
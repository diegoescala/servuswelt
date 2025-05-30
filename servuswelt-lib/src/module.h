#pragma once

#include "route.h"
#include <vector>

namespace servuswelt {
    class Module {
        public:
            virtual ~Module() = default;
            virtual const std::vector<Route> getRoutes() const = 0;
    };
} // namespace servuswelt
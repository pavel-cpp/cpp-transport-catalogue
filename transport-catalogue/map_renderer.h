#pragma once

#include <set>
#include <string>

#include "domain.h"
#include "geo.h"
#include "svg.h"

namespace renderer {

    struct Settings {
        double width_{};
        double heigth_{};
        double padding_{};
    };

    class MapRenderer final {
    public:
        MapRenderer() = delete;

        explicit MapRenderer(const Settings& settings);

        void AddBus(const Bus& bus);

        std::string Render(std::ostream& out);

    private:

        Settings settings_;

        std::set<Bus> buses_;

    };
}

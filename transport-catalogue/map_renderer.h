#pragma once

#include <array>
#include <set>
#include <string>

#include "domain.h"
#include "geo.h"
#include "svg.h"

namespace renderer {

    struct Settings {
        double width_{};
        double height_{};

        double padding_{};

        double line_width_{};
        double stop_radius_{};

        size_t bus_label_font_size_{};
        svg::Point bus_label_offset_{};

        size_t stop_label_font_size_{};
        svg::Point stop_label_offset_{};

        svg::Color underlayer_color_{};
        double underlayer_width_{};

        std::vector<svg::Color> color_palette_{};
    };

    class MapRenderer final {
    public:
        MapRenderer() = delete;

        explicit MapRenderer(Settings settings);

        void AddBus(const Bus &bus);

        void Render(std::ostream &out);

    private:
        svg::Point CoordinatesToPoint(geo::Coordinates coords) const;

        void CalculateZoomCoef();

        Settings settings_;

        std::set<Bus> buses_;

        double zoom_coef_{};
        double min_lng_;
        double max_lat_;
    };
}

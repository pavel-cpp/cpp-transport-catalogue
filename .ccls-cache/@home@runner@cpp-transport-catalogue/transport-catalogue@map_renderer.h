#pragma once

#include <array>
#include <set>
#include <string>

#include "domain.h"
#include "geo.h"
#include "svg.h"

namespace renderer {

    // {
    //   "width": 1200.0,
    //   "height": 1200.0,

    //   "padding": 50.0,

    //   "line_width": 14.0,
    //   "stop_radius": 5.0,

    //   "bus_label_font_size": 20,
    //   "bus_label_offset": [7.0, 15.0],

    //   "stop_label_font_size": 20,
    //   "stop_label_offset": [7.0, -3.0],

    //   "underlayer_color": [255, 255, 255, 0.85],
    //   "underlayer_width": 3.0,

    //   "color_palette": [
    //     "green",
    //     [255, 160, 0],
    //     "red"
    //   ]
    // }

    struct Settings {
        double width_{};
        double height_{};
    
        double padding_{};

        double line_width_{};
        double stop_radius_{};

        size_t bus_label_font_size_{};
        std::pair<double, double> bus_label_offset_{};

        size_t stop_label_font_size_{};
        std::pair<double, double> stop_label_offset_{};

        svg::Rgba underlayer_color_{};
        double underlayer_width_{};

        std::array<svg::Color, 3> color_palette_{};
    }
    

    class MapRenderer final {
    public:
        MapRenderer() = delete;

        explicit MapRenderer(Settings settings);

        void AddBus(const Bus& bus);

        void Render(std::ostream& out);

    private:

        std::pair<double, double> CoordinatesToPoint(geo::Coordinates coords) const;
        
        void CalculateZoomCoef();

        Settings settings_;

        std::set<Bus> buses_;

        double zoom_coef_{};
        double min_lng_;
        double max_lat_;
    };
}

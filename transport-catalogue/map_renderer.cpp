#include "map_renderer.h"

#include <utility>
#include <vector>

namespace renderer {

    MapRenderer::MapRenderer(Settings settings) :
        settings_(std::move(settings)) {
    }

    void MapRenderer::AddBus(const Bus &bus) {
        buses_.insert(bus);
    }

    template<typename Comp>
    std::vector<geo::Coordinates> ExtractAllCoordinates(const std::set<Bus, Comp> &buses) {
        std::vector<geo::Coordinates> all_coordinates;
        for (const Bus &bus: buses) {
            for (const Stop *stop: bus.route_) {
                all_coordinates.push_back(stop->position_);
            }
        }
        return all_coordinates;
    }

    void MapRenderer::Render(svg::Document &svg_out) const {
        auto coordinates = ExtractAllCoordinates(buses_);
        const SphereProjector projector(coordinates.begin(), coordinates.end(), settings_.width_, settings_.height_,
                                        settings_.padding_);

        size_t color_number = 0;
        for (const Bus &bus: buses_) {
            svg::Polyline route_line;

            for (const Stop *stop: bus.route_) {
                route_line.AddPoint(projector(stop->position_));
            }

            svg_out.Add(
                    route_line
                    .SetFillColor("none")
                    .SetStrokeColor(settings_.color_palette_[color_number])
                    .SetStrokeWidth(settings_.line_width_)
                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                    );

            /*{
                svg::Text bus_label;
                bus_label.SetData(bus.name_)
                        .SetFillColor(settings_.color_palette_[color_number])
                        .SetOffset(settings_.bus_label_offset_)
                        .SetFontSize(settings_.bus_label_font_size_);
                svg_out.Add(bus_label.SetPosition(projector(bus.route_.front()->position_)));
                svg_out.Add(bus_label.SetPosition(projector(bus.route_.back()->position_)));
            }

            svg::Circle stop_point;
            stop_point.SetRadius(settings_.stop_radius_).SetFillColor(settings_.color_palette_[color_number]);
            svg::Text stop_label;
            stop_label.SetFontSize(settings_.stop_label_font_size_).SetOffset(settings_.stop_label_offset_);
            for (const Stop *stop: bus.route_) {
                const auto position = projector(stop->position_);
                svg_out.Add(stop_point.SetCenter(position));
                svg_out.Add(stop_label.SetPosition(position).SetData(stop->name_));
            }*/
            ++color_number;
        }
    }

} // renderer

#include "map_renderer.h"

#include <utility>
#include <vector>
#include <unordered_set>

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

    class StopnameComparator {
    public:
        bool operator()(const Stop *lhs, const Stop *rhs) const {
            return lhs->name_ < rhs->name_;
        }
    };

    void MapRenderer::Render(svg::Document &svg_out) const {
        auto coordinates = ExtractAllCoordinates(buses_);
        const SphereProjector projector(coordinates.begin(), coordinates.end(), settings_.width_, settings_.height_,
                                        settings_.padding_);
        RenderLines(svg_out, projector);
        RenderBusnames(svg_out, projector);
        RenderCirclesAndStopnames(svg_out, projector);
    }

    void MapRenderer::RenderLines(svg::Document &svg_out, const SphereProjector &projector) const {
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
            color_number = (color_number + 1) % settings_.color_palette_.size();
        }
    }

    void MapRenderer::RenderBusnames(svg::Document &svg_out, const SphereProjector &projector) const {
        size_t color_number = 0;

        for (const Bus &bus: buses_) {
            svg::Text bus_label = svg::Text()
                    .SetFillColor(settings_.color_palette_[color_number])
                    .SetOffset(settings_.bus_label_offset_)
                    .SetFontSize(settings_.bus_label_font_size_)
                    .SetFontFamily("Verdana")
                    .SetFontWeight("bold");

            svg::Text bus_label_underlayer = bus_label;
            bus_label_underlayer
                    .SetFillColor(settings_.underlayer_color_)
                    .SetStrokeColor(settings_.underlayer_color_)
                    .SetStrokeWidth(settings_.underlayer_width_)
                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            svg_out.Add(bus_label_underlayer.SetPosition(projector(bus.route_.front()->position_)).SetData(bus.name_));
            svg_out.Add(bus_label.SetPosition(projector(bus.route_.front()->position_)).SetData(bus.name_));

            if (!bus.is_roundtrip_ && bus.route_.size() != 1 &&
                bus.route_.front()->position_ != bus.route_[bus.route_.size() / 2]->position_) {
                svg_out.Add(
                        bus_label_underlayer.SetPosition(projector(bus.route_[bus.route_.size() / 2]->position_)));
                svg_out.Add(bus_label.SetPosition(projector(bus.route_[bus.route_.size() / 2]->position_)));
            }


            color_number = (color_number + 1) % settings_.color_palette_.size();
        }
    }

    void MapRenderer::RenderCirclesAndStopnames(svg::Document &svg_out, const SphereProjector &projector) const {
        std::set<const Stop *, StopnameComparator> stops_to_render;
        for (const Bus &bus: buses_) {
            stops_to_render.insert(bus.route_.begin(), bus.route_.end());
        }

        svg::Circle stop_point = svg::Circle()
                .SetRadius(settings_.stop_radius_)
                .SetFillColor("white");
        for (const Stop *stop: stops_to_render) {
            svg_out.Add(stop_point.SetCenter(projector(stop->position_)));
        }

        svg::Text stop_label = svg::Text()
                .SetFontSize(settings_.stop_label_font_size_)
                .SetOffset(settings_.stop_label_offset_)
                .SetFontFamily("Verdana")
                .SetFillColor("black");
        svg::Text stop_label_underlayer = stop_label;
        stop_label_underlayer
                .SetFillColor(settings_.underlayer_color_)
                .SetStrokeColor(settings_.underlayer_color_)
                .SetStrokeWidth(settings_.underlayer_width_)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        for (const Stop *stop: stops_to_render) {
            svg_out.Add(stop_label_underlayer.SetPosition(projector(stop->position_)).SetData(stop->name_));
            svg_out.Add(stop_label.SetPosition(projector(stop->position_)).SetData(stop->name_));
        }
    }


} // renderer

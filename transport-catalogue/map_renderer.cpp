#include "map_renderer.h"

#include <algorithm>
#include <utility>

namespace renderer {

    MapRenderer::MapRenderer(Settings settings) :
        settings_(settings) {
    }

    void MapRenderer::AddBus(const Bus &bus) {
        buses_.insert(bus);
    }

    void MapRenderer::CalculateZoomCoef() {
        geo::Coordinates min_coords{};
        geo::Coordinates max_coords{};
        for (const Bus &bus: buses_) {

            const auto [min_lat, max_lat] = std::minmax_element(
                    bus.route_.begin(),
                    bus.route_.end(),
                    [](const Stop *lhs, const Stop *rhs) {
                        return lhs->position_.lat < rhs->position_.lat;
                    }
                    );
            const auto [min_lng, max_lng] = std::minmax_element(
                    bus.route_.begin(),
                    bus.route_.end(),
                    [](const Stop *lhs, const Stop *rhs) {
                        return lhs->position_.lng < rhs->position_.lng;
                    }
                    );

            min_coords.lat = std::min(min_coords.lat, (*min_lat)->position_.lat);
            min_coords.lng = std::min(min_coords.lng, (*min_lng)->position_.lng);

            max_coords.lat = std::max(max_coords.lat, (*max_lat)->position_.lat);
            max_coords.lng = std::max(max_coords.lng, (*max_lng)->position_.lng);
        }

        min_lng_ = min_coords.lng;
        max_lat_ = max_coords.lat;

        if (max_coords.lng - min_coords.lng && max_coords.lat - min_coords.lat) {
            double width_zoom_coef = (settings_.width_ - 2 * settings_.padding_) / (max_coords.lng - min_coords.lng);
            double height_zoom_coef = (settings_.height_ - 2 * settings_.padding_) / (max_coords.lat - min_coords.lat);
            zoom_coef_ = std::min(width_zoom_coef, height_zoom_coef);
        }
    }

    svg::Point MapRenderer::CoordinatesToPoint(geo::Coordinates coords) const {
        return {
                (coords.lng - min_lng_) * zoom_coef_ + settings_.padding_,
                (max_lat_ - coords.lat) * zoom_coef_ + settings_.padding_
        };
    }

    void MapRenderer::Render(std::ostream &out) {
        CalculateZoomCoef();

        svg::Document map_render;

        size_t color_number = 0;
        for (const Bus &bus: buses_) {
            svg::Polyline route_line;
            svg::Circle stop_point;
            stop_point.SetRadius(settings_.stop_radius_).SetFillColor(settings_.color_palette_[color_number]);
            svg::Text stop_label;
            stop_label.SetFontSize(settings_.stop_label_font_size_).SetOffset(settings_.stop_label_offset_);
            for (const Stop *stop: bus.route_) {
                auto position = CoordinatesToPoint(stop->position_);
                route_line.AddPoint(position);
                map_render.Add(stop_point.SetCenter(position));
                map_render.Add(stop_label.SetPosition(position).SetData(stop->name_));
            }
            svg::Text bus_label;
            bus_label.SetData(bus.name_)
                    .SetFillColor(settings_.color_palette_[color_number])
                    .SetOffset(settings_.bus_label_offset_)
                    .SetFontSize(settings_.bus_label_font_size_);
            map_render.Add(bus_label.SetPosition(CoordinatesToPoint(bus.route_.front()->position_)));
            map_render.Add(bus_label.SetPosition(CoordinatesToPoint(bus.route_.back()->position_)));
            map_render.Add(
                    route_line
                    .SetFillColor(settings_.color_palette_[color_number])
                    .SetStrokeWidth(settings_.width_)
                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                    );
        }

        map_render.Render(out);

    }

} // renderer

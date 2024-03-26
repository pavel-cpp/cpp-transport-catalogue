#include "map_renderer.h"

#include <algorithm>
#include <utility>

namespace renderer {
    
    MapRenderer::MapRenderer(Settings settings) : settings_(settings){}

    void MapRenderer::AddBus(const Bus &bus) {
        buses_.insert(bus);
    }

    void MapRenderer::CalculateZoomCoef() {
        geo::Coordinates min_coords;
        geo::Coordinates max_coords;
        for(const Bus& bus: buses_){

            const auto [min_lat, max_lat] = std::minmax_element(
                bus.route_.begin(),
                bus.route_.end(),
                [](const Stop* lhs, const Stop* rhs) {
                    return lhs->position_.lat < rhs->position_.lat;
                }
            );
            const auto [min_lng, max_lng] = std::minmax_element(
                bus.route_.begin(),
                bus.route_.end(),
                [](const Stop* lhs, const Stop* rhs) {
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
        
        if(max_coords.lng - min_coords.lng && max_coords.lat - min_coords.lat){
            double width_zoom_coef = (width_ - 2 * padding_) / (max_coords.lng - min_coords.lng);
            double height_zoom_coef = (height_ - 2 * padding_) / (max_coords.lat - min_coords.lat);
            zoom_coef_ = std::min(width_zoom_coef, height_zoom_coef);
        }
        
    }

    std::pair<double, double> MapRenderer::CoordinatesToPoint(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lng_) * zoom_coef_ + padding_,
            (max_lat_ - coords.lat) * zoom_coef_ + padding_
        };
    }

    void MapRenderer::Render(std::ostream &out) {
        CalculateZoomCoef();

        
            
    }

} // renderer

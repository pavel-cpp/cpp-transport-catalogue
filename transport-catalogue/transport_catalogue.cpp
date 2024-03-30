#include "transport_catalogue.h"

// Other
#include <stdexcept>

using namespace std;

void TransportCatalogue::AddStop(std::string_view name, geo::Coordinates position) {
    AddStopImpl({name, position});
}

void TransportCatalogue::AddStopImpl(const Stop &stop) {
    stops_.push_back(stop);
    auto new_stop = &stops_.back();
    stop_to_buses_[new_stop];
    stopname_to_stop_[new_stop->name_] = new_stop;
}

void TransportCatalogue::AddRoute(string_view bus_name, const vector<string_view> &stopnames, bool is_roundtrip) {
    buses_.emplace_back(bus_name);
    Bus *new_bus = bus_routes_[buses_.back().name_] = &buses_.back();
    new_bus->is_roundtrip_ = is_roundtrip;
    new_bus->route_.reserve(stopnames.size());
    for (string_view stopname: stopnames) {
        AssociateStopWithBus(stopname_to_stop_[stopname], new_bus);
        bus_routes_[new_bus->name_]->route_.push_back(stopname_to_stop_[stopname]);
    }
}

Bus TransportCatalogue::FindRoute(string_view bus_name) const {
    return *bus_routes_.at(bus_name);
}

const Stop &TransportCatalogue::FindStop(string_view stop_name) const {
    return *stopname_to_stop_.at(stop_name);
}

RouteInfo TransportCatalogue::BusRouteInfo(string_view bus_name) const {
    double native_length = CalculateNativeRouteLength(string(bus_name));
    double real_length = CalculateRealRouteLength(string(bus_name));
    return {
            bus_routes_.at(string(bus_name))->route_.size(),
            CountUniqueRouteStops(string(bus_name)),
            real_length,
            real_length / native_length
    };
}

double TransportCatalogue::CalculateRealRouteLength(std::string_view bus_name) const {
    double route_length = 0;
    const auto& route = bus_routes_.at(bus_name)->route_;
    const Stop *last_stop = route.front();
    for (const auto stop: route) {
        if(stop == last_stop) {
            continue;
        }
        try {
            route_length += static_cast<double>(stop_to_near_stop_.at(make_pair(const_cast<Stop *>(last_stop), const_cast<Stop *>(stop))));
        } catch (std::out_of_range &) {
            route_length += static_cast<double>(stop_to_near_stop_.at(make_pair(const_cast<Stop *>(stop), const_cast<Stop *>(last_stop))));
        }

        last_stop = stop;
    }
    return route_length;
}

double TransportCatalogue::CalculateNativeRouteLength(string_view bus_name) const {
    double route_length = 0;
    bool is_first = true;
    geo::Coordinates past_position{};
    for (const auto &stop: bus_routes_.at(bus_name)->route_) {
        if (!is_first) {
            route_length += ComputeDistance(past_position, stop->position_);
        } else {
            is_first = false;
        }
        past_position = stop->position_;
    }
    return route_length;
}

size_t TransportCatalogue::CountUniqueRouteStops(string_view bus_name) const {
    size_t unique_stops_count = 0;

    // Проверяем, есть ли маршрут с указанным именем
    if (bus_routes_.find(bus_name) != bus_routes_.end()) {

        const std::vector<const Stop *> &route = bus_routes_.at(bus_name)->route_;

        unique_stops_count = unordered_set<const Stop *>(route.begin(), route.end()).size();

    }

    return unique_stops_count;
}

void TransportCatalogue::AssociateStopWithBus(Stop *stop, const Bus *bus) {
    stop_to_buses_[stop].insert(bus->name_);
}

TransportCatalogue::SortedBuses TransportCatalogue::StopInfo(std::string_view stop_name) const {
    const auto &buses = stop_to_buses_.at(stopname_to_stop_.at(stop_name));
    return {buses.begin(), buses.end()};
}

void TransportCatalogue::AddDistance(std::string_view stopname_from, std::string_view stopname_to, size_t distance) {
    stop_to_near_stop_[{
            stopname_to_stop_.at(stopname_from),
            stopname_to_stop_.at(stopname_to)
    }] = distance;
}

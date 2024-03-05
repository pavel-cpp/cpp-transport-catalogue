#include "transport_catalogue.h"

// Other
#include <stdexcept>

using namespace std;

void TransportCatalogue::AddStop(std::string_view name, Coordinates position) {
    AddStopImpl({name, position});
}

// Добавляет остановку и ближайшие к ней
void TransportCatalogue::AddStop(std::string_view name, Coordinates position,
                                 const std::unordered_map<std::string_view, size_t> &near_stops) {
    AddStop(name, position);
    auto current_stop = stopname_to_stop_.find(name);
    for (const auto &[stopname, route_length]: near_stops) {
        auto found = stopname_to_stop_.find(stopname);
        if (found == stopname_to_stop_.end()) {
            AddStop(stopname);
            found = stopname_to_stop_.find(stopname);
        }
        stop_to_near_stop_[make_pair(current_stop->second, found->second)] = route_length;
    }
}

void TransportCatalogue::AddStopImpl(const TransportCatalogue::Stop &stop) {
    auto found = stopname_to_stop_.find(stop.name_);
    if (found != stopname_to_stop_.end()) {
        if (stop.position_.has_value()) {
            found->second->position_ = stop.position_.value();
        }
        return;
    }
    if (stop.position_.has_value()) {
        stops_.emplace_back(stop.name_, stop.position_.value());
    } else {
        stops_.emplace_back(stop.name_);
    }
    auto new_stop = &stops_.back();
    stop_to_buses_[new_stop];
    stopname_to_stop_[new_stop->name_] = new_stop;
}

void TransportCatalogue::AddRoute(string_view bus_name, const vector<string_view> &stopnames) {
    buses_.emplace_back(bus_name);
    Bus *new_bus = bus_routes_[buses_.back().name_] = &buses_.back();
    new_bus->route_.reserve(stopnames.size());
    for (string_view stopname: stopnames) {
        AddStop(stopname);
        AssociateStopWithBus(stopname_to_stop_[stopname], new_bus);
        bus_routes_[new_bus->name_]->route_.push_back(stopname_to_stop_[stopname]);
    }
}

TransportCatalogue::Bus TransportCatalogue::FindRoute(string_view bus_name) const {
    return *bus_routes_.at(bus_name);
}

const TransportCatalogue::Stop &TransportCatalogue::FindStop(string_view stop_name) const {
    return *stopname_to_stop_.at(stop_name);
}

TransportCatalogue::RouteInfo TransportCatalogue::BusRouteInfo(string_view bus_name) const {
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
    bool is_first = true;
    Stop *last_stop{};
    for (const auto &stop: bus_routes_.at(bus_name)->route_) {
        if (!is_first) {
            try {
                route_length += stop_to_near_stop_.at(make_pair(last_stop, stop));
            } catch (std::out_of_range &) {
                route_length += stop_to_near_stop_.at(make_pair(stop, last_stop));
            }
        } else {
            is_first = false;
        }
        last_stop = stop;
    }
    return route_length;
}

double TransportCatalogue::CalculateNativeRouteLength(string_view bus_name) const {
    double route_length = 0;
    bool is_first = true;
    Coordinates past_position{};
    for (const auto &stop: bus_routes_.at(bus_name)->route_) {
        if (!is_first) {
            route_length += ComputeDistance(past_position, stop->position_.value());
        }
        if (stop->position_.has_value()) {
            past_position = stop->position_.value();
            is_first = false;
        }
    }
    return route_length;
}

size_t TransportCatalogue::CountUniqueRouteStops(string_view bus_name) const {
    size_t unique_stops_count = 0;

    // Проверяем, есть ли маршрут с указанным именем
    if (bus_routes_.find(bus_name) != bus_routes_.end()) {

        const std::vector<Stop *> &route = bus_routes_.at(bus_name)->route_;

        unique_stops_count = unordered_set<Stop *>(route.begin(), route.end()).size();

    }

    return unique_stops_count;
}

void TransportCatalogue::AssociateStopWithBus(TransportCatalogue::Stop *stop, TransportCatalogue::Bus *bus) {
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

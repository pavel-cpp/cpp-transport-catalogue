#pragma once

// Other
#include <string>

// STL
#include <deque>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Local
#include "geo.h"

class TransportCatalogue {

    struct Stop {
        std::string name_;
        std::optional<Coordinates> position_;
        std::unordered_map<std::string_view, size_t> near_stops_;

        Stop() = default;

        explicit Stop(std::string_view name) : name_(name) {}

        Stop(std::string_view name, Coordinates position) : name_(name), position_(position) {}

        bool operator==(const Stop &stop) const {
            return name_ == stop.name_ && position_ == stop.position_;
        }

    };

    using RouteCopy = std::vector<Stop>;
    using Route = std::vector<Stop *>;
    using Buses = std::unordered_set<std::string_view>;
    using SortedBuses = std::set<std::string_view>;

    struct Bus {
        std::string name_;
        Route route_;

        Bus() = default;

        explicit Bus(std::string_view name) : name_(name) {}

        Bus(std::string_view name, const Route &route) : name_(name), route_(route) {}

        bool operator==(const Bus &bus) const {
            return name_ == bus.name_
                   && std::equal(route_.begin(), route_.end(), bus.route_.begin(), bus.route_.end());
        }

    };

public:

    struct RouteInfo {
        size_t total_stops;
        size_t unique_stops;
        double length;
        double curvature;
    };

    void AddStop(std::string_view name, Coordinates position);

    void AddStop(std::string_view name, Coordinates position, const std::unordered_map<std::string_view, size_t>& near_stops);

    void AddRoute(std::string_view bus_name, const std::vector<std::string_view> &stopnames);

    [[nodiscard]] RouteCopy FindRoute(std::string_view bus_name) const;

    [[nodiscard]] const Stop &FindStop(std::string_view stop_name) const;

    // TODO(Pavel): Обновить для получения информации с curvature
    [[nodiscard]] RouteInfo BusRouteInfo(std::string_view bus_name) const;

    [[nodiscard]] SortedBuses StopInfo(std::string_view stop_name) const;

private:

    void AssociateStopWithBus(Stop *stop, Bus *bus);

    void AddStop(std::string_view name);

    void AddStopImpl(const Stop &stop);

    // TODO(Pavel): Добавить CalculateRealRouteLength

    [[nodiscard]] double CalculateNativeRouteLength(std::string_view bus_name) const;

    [[nodiscard]] size_t CountUniqueRouteStops(std::string_view bus_name) const;

    std::unordered_map<std::string_view, Stop *> stopname_to_stop_;
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, Bus *> bus_routes_;
    std::deque<Bus> buses_;
    std::unordered_map<Stop *, Buses> stop_to_buses_;

};
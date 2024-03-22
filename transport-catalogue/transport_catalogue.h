#pragma once

// Other
#include <string>
#include <cstdint>

// STL
#include <deque>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Local
#include "domain.h"
#include "geo.h"

class TransportCatalogue {

    class PairStopHasher {
    private:
        std::hash<Stop *> hasher;
    public:
        uint64_t operator()(std::pair<Stop *, Stop *> stop_pair) const {
            return hasher(stop_pair.first) + hasher(stop_pair.second);
        }
    };

    using Buses = std::unordered_set<std::string_view>;
    using SortedBuses = std::set<std::string_view>;

public:

    struct RouteInfo {
        size_t total_stops;
        size_t unique_stops;
        double length;
        double curvature;
    };

    void AddStop(std::string_view name, geo::Coordinates position);

    void AddDistance(std::string_view stopname_from, std::string_view stopname_to, size_t distance);

    void AddRoute(std::string_view bus_name, const std::vector<std::string_view> &stopnames);

    [[maybe_unused]] [[nodiscard]] const Stop &FindStop(std::string_view stop_name) const;

    [[maybe_unused]] [[nodiscard]] Bus FindRoute(std::string_view bus_name) const;

    [[nodiscard]] RouteInfo BusRouteInfo(std::string_view bus_name) const;

    [[nodiscard]] SortedBuses StopInfo(std::string_view stop_name) const;

private:

    void AssociateStopWithBus(Stop *stop, const Bus *bus);

    void AddStopImpl(const Stop &stop);

    [[nodiscard]] double CalculateRealRouteLength(std::string_view bus_name) const;

    [[nodiscard]] double CalculateNativeRouteLength(std::string_view bus_name) const;

    [[nodiscard]] size_t CountUniqueRouteStops(std::string_view bus_name) const;

    std::unordered_map<std::string_view, Stop *> stopname_to_stop_;
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, Bus *> bus_routes_;
    std::deque<Bus> buses_;
    std::unordered_map<Stop *, Buses> stop_to_buses_;
    std::unordered_map<std::pair<Stop *, Stop *>, size_t, PairStopHasher> stop_to_near_stop_;
};
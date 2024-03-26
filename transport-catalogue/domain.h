#pragma once

#include <string>
#include <vector>

#include "geo.h"

struct Stop {
    std::string name_;
    geo::Coordinates position_;

    Stop() = default;

    explicit Stop(std::string_view name) : name_(name) {
    }

    Stop(std::string_view name, geo::Coordinates position) : name_(name), position_(position) {
    }

    bool operator==(const Stop &stop) const {
        return name_ == stop.name_ && position_ == stop.position_;
    }
};

struct Bus {
    std::string name_;
    std::vector<Stop *> route_;

    Bus() = default;

    explicit Bus(std::string_view name) : name_(name) {
    }

    Bus(std::string_view name, const std::vector<Stop *> &route) : name_(name), route_(route) {
    }

    bool operator==(const Bus &bus) const {
        return name_ == bus.name_
               && std::equal(route_.begin(), route_.end(), bus.route_.begin(), bus.route_.end());
    }
};

struct RouteInfo {
    size_t total_stops;
    size_t unique_stops;
    double length;
    double curvature;
};

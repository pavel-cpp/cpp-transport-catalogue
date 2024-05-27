#pragma once

#include <memory>

#include "graph.h"
#include "router.h"

#include "transport_catalogue.h"

class TransportRouter {
public:

    using RouteInfo = graph::Router<double>::RouteInfo;

    explicit TransportRouter(uint8_t bus_wait_time, double bus_velocity, const TransportCatalogue& catalogue);

    [[nodiscard]] std::optional<RouteInfo> FindRoute(std::string_view stop_from, std::string_view stop_to) const;

private:

    uint8_t bus_wait_time_ {0};
    double bus_velocity_ {0.0};

    graph::DirectedWeightedGraph<double> graph_ {};
    std::map<std::string, graph::VertexId> stop_ids_ {};
    std::unique_ptr<graph::Router<double>> router_ {};

};

class TransportRouterBuilder {
public:

    TransportRouterBuilder() = delete;
    explicit TransportRouterBuilder(const TransportCatalogue& catalogue) : catalogue_(catalogue) {}
    TransportRouterBuilder(const TransportRouterBuilder&) = default;
    TransportRouterBuilder(TransportRouterBuilder&&) = default;

    TransportRouterBuilder& SetBusWaitTime(uint8_t value) {
        bus_wait_time_ = value;
        return *this;
    }

    TransportRouterBuilder& SetBusVelocity(uint8_t value) {
        bus_velocity_ = value;
        return *this;
    }

private:

    const TransportCatalogue& catalogue_ {};
    uint8_t bus_wait_time_ {0};
    double bus_velocity_ {0.0};

};

#pragma once

#include <memory>

#include "graph.h"
#include "router.h"

#include "transport_catalogue.h"

struct TransportRouterSettings {
    uint8_t bus_wait_time;
    double bus_velocity;
};

class TransportRouter final {
public:
    using RouteInfo = graph::Router<double>::RouteInfo;
    using Graph = graph::DirectedWeightedGraph<double>;

    explicit TransportRouter(const TransportRouterSettings& settings, const TransportCatalogue &catalogue);

    [[nodiscard]] std::optional<RouteInfo> FindRoute(std::string_view stop_from, std::string_view stop_to) const;

    // Испольуется для составления отчета и чтобы не городить optional пар
    [[nodiscard]] Graph GetGraph() const;

private:
    void FillGraphStops(const TransportCatalogue &catalogue,
                        graph::VertexId &vertex_id,
                        graph::DirectedWeightedGraph<double> &stops_graph);

    void FillGraphBuses(const TransportCatalogue &catalogue,
                        graph::VertexId &vertex_id,
                        graph::DirectedWeightedGraph<double> &stops_graph);

    TransportRouterSettings settings_{};

    graph::DirectedWeightedGraph<double> graph_{};
    std::map<std::string, graph::VertexId> stop_ids_{};
    std::unique_ptr<graph::Router<double>> router_{};

};

class TransportRouterBuilder final {
public:
    TransportRouterBuilder() = delete;

    explicit TransportRouterBuilder(const TransportCatalogue &catalogue) :
        catalogue_(catalogue) {
    }

    TransportRouterBuilder(const TransportRouterBuilder &) = default;
    TransportRouterBuilder(TransportRouterBuilder &&) = default;

    TransportRouterBuilder &SetBusWaitTime(uint8_t value) noexcept {
        settings_.bus_wait_time = value;
        return *this;
    }

    TransportRouterBuilder &SetBusVelocity(uint8_t value) noexcept {
        settings_.bus_velocity = value;
        return *this;
    }

    TransportRouter Build() const noexcept {
        return TransportRouter{settings_, catalogue_};
    }

private:
    const TransportCatalogue &catalogue_{};
    TransportRouterSettings settings_{};

};

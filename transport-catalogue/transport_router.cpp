#include "transport_router.h"

#include <memory>

TransportRouter::TransportRouter(
        const TransportRouterSettings &settings,
        const TransportCatalogue &catalogue
        ) : settings_(settings) {

    graph::DirectedWeightedGraph<double> stops_graph(catalogue.GetAllSortedStops().size() * 2);
    graph::VertexId vertex_id = 0;

    FillGraphStops(catalogue, vertex_id, stops_graph);
    FillGraphBuses(catalogue, vertex_id, stops_graph);

    graph_ = std::move(stops_graph);
    router_ = std::make_unique<graph::Router<double>>(graph_);
}

std::optional<TransportRouter::RouteInfo>
TransportRouter::FindRoute(
        std::string_view stop_from,
        std::string_view stop_to
        ) const {
    return router_->BuildRoute(stop_ids_.at(std::string(stop_from)), stop_ids_.at(std::string(stop_to)));
}

TransportRouter::Graph TransportRouter::GetGraph() const {
    return graph_;
}

void TransportRouter::FillGraphStops(
        const TransportCatalogue &catalogue,
        graph::VertexId &vertex_id,
        graph::DirectedWeightedGraph<double> &stops_graph
        ) {
    for (const auto &[stop_name, stop_info]: catalogue.GetAllSortedStops()) {
        stop_ids_[stop_info->name_] = vertex_id;
        stops_graph.AddEdge({
                stop_info->name_,
                0,
                vertex_id,
                ++vertex_id,
                static_cast<double>(settings_.bus_wait_time)
        });
        ++vertex_id;
    }
}

void TransportRouter::FillGraphBuses(
        const TransportCatalogue &catalogue,
        graph::VertexId &vertex_id,
        graph::DirectedWeightedGraph<double> &stops_graph
        ) {
    for (const auto &[bus_name, bus_info]: catalogue.GetAllSortedBuses()) {
        const auto &stops = bus_info->route_;
        size_t stops_count = stops.size();
        for (size_t i = 0; i < stops_count; ++i) {
            for (size_t j = i + 1; j < stops_count; ++j) {
                const Stop *stop_from = stops[i];
                const Stop *stop_to = stops[j];
                size_t dist_sum = 0;
                size_t dist_sum_inverse = 0;
                for (size_t k = i + 1; k <= j; ++k) {
                    dist_sum += catalogue.Distance(stops[k - 1], stops[k]);
                    dist_sum_inverse += catalogue.Distance(stops[k], stops[k - 1]);
                }
                stops_graph.AddEdge({bus_info->name_,
                                     j - i,
                                     stop_ids_.at(stop_from->name_) + 1,
                                     stop_ids_.at(stop_to->name_),
                                     static_cast<double>(dist_sum) / (settings_.bus_velocity * (100.0 / 6.0))});

                if (!bus_info->is_roundtrip_) {
                    stops_graph.AddEdge({bus_info->name_,
                                         j - i,
                                         stop_ids_.at(stop_to->name_) + 1,
                                         stop_ids_.at(stop_from->name_),
                                         static_cast<double>(dist_sum_inverse) / (
                                             settings_.bus_velocity * (100.0 / 6.0))});
                }
            }
        }
    }
}

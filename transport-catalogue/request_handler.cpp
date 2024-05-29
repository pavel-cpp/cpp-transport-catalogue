#include "request_handler.h"

RequestHandler::RequestHandler(const TransportCatalogue &db, const renderer::MapRenderer &renderer, const TransportRouter& router) :
    catalogue_(db), renderer_(renderer), router_(router) {
}

std::optional<RouteInfo> RequestHandler::GetBusStat(const std::string_view &bus_name) const {
    try {
        return catalogue_.BusRouteInfo(bus_name);
    } catch (std::out_of_range &) {
        return std::nullopt;
    }
}

std::set<std::string_view> RequestHandler::GetBusesByStop(const std::string_view &stop_name) const {
    return catalogue_.StopInfo(stop_name);
}

svg::Document RequestHandler::RenderMap() const {
    svg::Document doc;
    renderer_.Render(doc);
    return doc;
}

std::optional<TransportRouter::RouteInfo> RequestHandler::FindRoute(std::string_view from, std::string_view to) const {
    return router_.FindRoute(from, to);
}

TransportRouter::Graph RequestHandler::GetRouterGraph() const {
    return router_.GetGraph();
}

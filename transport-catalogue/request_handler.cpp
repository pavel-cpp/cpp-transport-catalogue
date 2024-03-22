#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
RequestHandler::RequestHandler(const TransportCatalogue &db, const renderer::MapRenderer &renderer)
    : db_(db), renderer_(renderer) {}

std::optional<RouteInfo> RequestHandler::GetBusStat(const std::string_view &bus_name) const {
    try {
        return db_.BusRouteInfo(bus_name);
    } catch (std::out_of_range& ) {
        return std::nullopt;
    }
}

const std::unordered_set<Bus *> * RequestHandler::GetBusesByStop(const std::string_view &stop_name) const {

}

#pragma once

#include <optional>

#include "map_renderer.h"
#include "svg.h"
#include "transport_catalogue.h"
#include "transport_router.h"

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
class RequestHandler final {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& catalogue, const renderer::MapRenderer& renderer, const TransportRouter& router);

    // Возвращает информацию о маршруте (запрос Bus)
    [[nodiscard]] std::optional<RouteInfo> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    [[nodiscard]] std::set<std::string_view> GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    [[nodiscard]] svg::Document RenderMap() const;

    [[nodiscard]] std::optional<TransportRouter::RouteInfo> FindRoute(std::string_view from, std::string_view to) const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
    const TransportRouter& router_;
};

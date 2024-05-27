#include "json_reader.h"

#include <algorithm>
#include <sstream>

#include "json_builder.h"

void JsonReader::ReadData(std::istream &input) {
    document_ = json::Load(input);
}

// Функция, которая распаковывает соординаты
geo::Coordinates ExtractCoordinates(const json::Dict &dict) {
    return {dict.at("latitude").AsDouble(), dict.at("longitude").AsDouble()};
}

void JsonReader::ProcessBaseRequests(TransportCatalogue &db, renderer::MapRenderer &map) const {
    using namespace std::literals;
    const auto base_requests = document_.GetRoot().AsDict().at("base_requests"s).AsArray();

    // Добавляем остановки
    for (const auto &request: base_requests) {
        if (request.AsDict().at("type"s) == "Stop"s) {
            db.AddStop(request.AsDict().at("name"s).AsString(), ExtractCoordinates(request.AsDict()));
        }
    }

    // Добавляем дистанции между остановками
    for (const auto &request: base_requests) {
        if (request.AsDict().at("type"s) == "Stop"s) {
            std::string stopname = request.AsDict().at("name"s).AsString();
            for (const auto &[name, distance]: request.AsDict().at("road_distances"s).AsDict()) {
                db.AddDistance(stopname, name, distance.AsInt());
            }
        }
    }

    // Создаем и добавляем маршруты
    for (const auto &request: base_requests) {
        if (request.AsDict().at("type"s) == "Bus"s) {
            std::string busname = request.AsDict().at("name"s).AsString();
            std::vector<std::string_view> stops;
            for (const auto &stop: request.AsDict().at("stops"s).AsArray()) {
                stops.push_back(stop.AsString());
            }
            if (!request.AsDict().at("is_roundtrip"s).AsBool()) {
                std::vector<std::string_view> results(stops.begin(), stops.end());
                results.insert(results.end(), std::next(stops.rbegin()), stops.rend());
                stops = std::move(results);
            } else if (stops.front() != stops.back()) {
                stops.push_back(stops.front());
            }
            // Добавляем маршрут
            db.AddRoute(busname, stops, request.AsDict().at("is_roundtrip"s).AsBool());

            // Добавляем автобус в карту для дальнейшей отрисовки
            map.AddBus(db.FindRoute(busname));
        }
    }
}

void JsonReader::ProcessRoutingSettings(TransportCatalogue &db, TransportRouterBuilder &router_builder) const {
    using namespace std::literals;
    const auto routing_settings = document_.GetRoot().AsDict().at("routing_settings"s).AsDict();

    router_builder
            .SetBusVelocity(static_cast<uint8_t>(routing_settings.at("bus_velocity"s).AsDouble()))
            .SetBusWaitTime(routing_settings.at("bus_wait_time"s).AsInt());

}

void JsonReader::ProcessStatRequests(const RequestHandler &db, std::ostream &output) const {
    using namespace std::literals;
    const auto stat_requests = document_.GetRoot().AsDict().at("stat_requests"s).AsArray();

    RequestHandler handler(db);
    json::Builder response_builder{};
    response_builder.StartArray();
    for (const auto &request: stat_requests) {
        response_builder.StartDict();
        response_builder.Key("request_id"s).Value(request.AsDict().at("id"s).AsInt());

        if (request.AsDict().at("type"s).AsString() == "Bus"s) {
            auto route_info = handler.GetBusStat(request.AsDict().at("name"s).AsString());
            if (route_info.has_value()) {
                response_builder
                        .Key("route_length"s).Value(route_info->length)
                        .Key("curvature"s).Value(route_info->curvature)
                        .Key("stop_count"s).Value(static_cast<int>(route_info->total_stops))
                        .Key("unique_stop_count"s).Value(static_cast<int>(route_info->unique_stops));
            } else {
                response_builder.Key("error_message"s).Value("not found"s);
            }
        } else if (request.AsDict().at("type"s).AsString() == "Stop"s) {
            try {
                auto buses = handler.GetBusesByStop(request.AsDict().at("name"s).AsString());
                response_builder.Key("buses").StartArray();
                for (std::string_view bus: buses) {
                    response_builder.Value(std::string(bus));
                }
                response_builder.EndArray();
            } catch (std::out_of_range &) {
                response_builder.Key("error_message"s).Value("not found"s);
            }
        } else if (request.AsDict().at("type"s).AsString() == "Map"s) {
            std::stringstream ss;
            handler.RenderMap().Render(ss);
            response_builder.Key("map"s).Value(ss.str());
        }
        response_builder.EndDict();
    }
    response_builder.EndArray();
    json::Print(json::Document(response_builder.Build()), output);
}

// Функция для распаковки цвета
svg::Color ExtractColor(const json::Node &node) {
    if (node.IsString()) {
        return node.AsString();
    }
    if (node.IsArray()) {
        if (node.AsArray().size() == 3) {
            return svg::Rgb(node.AsArray()[0].AsInt(), node.AsArray()[1].AsInt(), node.AsArray()[2].AsInt());
        } else if (node.AsArray().size() == 4) {
            return svg::Rgba(node.AsArray()[0].AsInt(), node.AsArray()[1].AsInt(), node.AsArray()[2].AsInt(),
                             node.AsArray()[3].AsDouble());
        }
    }
    return {};
}

renderer::Settings JsonReader::GetRenderSettings() const {
    renderer::Settings settings;
    using namespace std::literals;
    const auto render_settings = document_.GetRoot().AsDict().at("render_settings"s).AsDict();

    settings.width_ = render_settings.at("width"s).AsDouble();
    settings.height_ = render_settings.at("height"s).AsDouble();
    settings.padding_ = render_settings.at("padding"s).AsDouble();
    settings.line_width_ = render_settings.at("line_width"s).AsDouble();
    settings.stop_radius_ = render_settings.at("stop_radius"s).AsDouble();
    settings.bus_label_font_size_ = render_settings.at("bus_label_font_size"s).AsInt();
    settings.bus_label_offset_ = {render_settings.at("bus_label_offset"s).AsArray().at(0).AsDouble(),
                                  render_settings.at("bus_label_offset"s).AsArray().at(1).AsDouble()};
    settings.stop_label_font_size_ = render_settings.at("stop_label_font_size"s).AsInt();
    settings.stop_label_offset_ = {render_settings.at("stop_label_offset"s).AsArray().at(0).AsDouble(),
                                   render_settings.at("stop_label_offset"s).AsArray().at(1).AsDouble()};
    settings.underlayer_color_ = ExtractColor(render_settings.at("underlayer_color"s));
    settings.underlayer_width_ = render_settings.at("underlayer_width"s).AsDouble();
    settings.color_palette_.reserve(render_settings.at("color_palette"s).AsArray().size());

    for (const auto &color: render_settings.at("color_palette"s).AsArray()) {
        settings.color_palette_.push_back(ExtractColor(color));
    }

    return settings;
}

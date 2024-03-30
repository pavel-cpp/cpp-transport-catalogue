#include "json_reader.h"

#include <algorithm>
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
void JsonReader::ReadData(std::istream &input) {
    document_ = json::Load(input);
}

geo::Coordinates ExtractCoordinates(const json::Dict &dict) {
    return {dict.at("latitude").AsDouble(), dict.at("longitude").AsDouble()};
}

void JsonReader::ProcessBaseRequests(TransportCatalogue &db, renderer::MapRenderer &map) const {
    using namespace std::literals;
    const auto base_requests = document_.GetRoot().AsMap().at("base_requests"s).AsArray();

    for (const auto &request: base_requests) {
        if (request.AsMap().at("type"s) == "Stop"s) {
            db.AddStop(request.AsMap().at("name"s).AsString(), ExtractCoordinates(request.AsMap()));
        }
    }

    for (const auto &request: base_requests) {
        if (request.AsMap().at("type"s) == "Stop"s) {
            std::string stopname = request.AsMap().at("name"s).AsString();
            for (const auto &[name, distance]: request.AsMap().at("road_distances"s).AsMap()) {
                db.AddDistance(stopname, name, distance.AsInt());
            }
        }
    }

    for (const auto &request: base_requests) {
        if (request.AsMap().at("type"s) == "Bus"s) {
            std::string busname = request.AsMap().at("name"s).AsString();
            std::vector<std::string_view> stops;
            for (const auto &stop: request.AsMap().at("stops"s).AsArray()) {
                stops.push_back(stop.AsString());
            }
            if (!request.AsMap().at("is_roundtrip"s).AsBool()) {
                std::vector<std::string_view> results(stops.begin(), stops.end());
                results.insert(results.end(), std::next(stops.rbegin()), stops.rend());
                stops = std::move(results);
            } else if (stops.front() != stops.back()) {
                stops.push_back(stops.front());
            }
            db.AddRoute(busname, stops, request.AsMap().at("is_roundtrip"s).AsBool());
            map.AddBus(db.FindRoute(busname));
        }
    }
}

json::Node AsJsonNode(const RouteInfo &route_info) {
    using namespace std::literals;
    json::Dict dict;
    dict["route_length"s] = route_info.length;
    dict["curvature"s] = route_info.curvature;
    dict["stop_count"s] = static_cast<int>(route_info.total_stops);
    dict["unique_stop_count"s] = static_cast<int>(route_info.unique_stops);
    return dict;
}

void JsonReader::ProcessStatRequests(const RequestHandler &db, std::ostream &output) const {
    using namespace std::literals;
    const auto stat_requests = document_.GetRoot().AsMap().at("stat_requests"s).AsArray();

    RequestHandler handler(db);
    json::Array responses;
    for (const auto &request: stat_requests) {
        json::Dict response;
        response["request_id"s] = request.AsMap().at("id"s).AsInt();

        if (request.AsMap().at("type"s).AsString() == "Bus"s) {
            auto route_info = handler.GetBusStat(request.AsMap().at("name"s).AsString());
            if (route_info.has_value()) {
                auto info = AsJsonNode(*route_info).AsMap();
                response.insert(info.begin(), info.end());
            } else {
                response["error_message"s] = "not found"s;
            }
        } else if (request.AsMap().at("type"s).AsString() == "Stop"s) {
            try {
                json::Array buses;
                for (std::string_view bus: handler.GetBusesByStop(request.AsMap().at("name"s).AsString())) {
                    buses.emplace_back(std::string(bus));
                }
                response["buses"s] = std::move(buses);
            } catch (std::out_of_range &) {
                response["error_message"s] = "not found"s;
            }
        }
        responses.emplace_back(std::move(response));
    }
    json::Print(json::Document(std::move(responses)), output);
}

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

void JsonReader::ProcessRenderSettings(renderer::Settings &settings) const {
    using namespace std::literals;
    const auto render_settings = document_.GetRoot().AsMap().at("render_settings"s).AsMap();

    settings.width_ = render_settings.at("width"s).AsDouble();
    settings.height_ = render_settings.at("height"s).AsDouble();
    settings.padding_ = render_settings.at("padding"s).AsDouble();
    settings.line_width_ = render_settings.at("line_width"s).AsDouble();
    settings.stop_radius_ = render_settings.at("stop_radius"s).AsDouble();
    settings.bus_label_font_size_ = render_settings.at("bus_label_font_size"s).AsInt();
    settings.bus_label_offset_ = {render_settings.at("bus_label_offset"s).AsArray()[0].AsDouble(),
                                  render_settings.at("bus_label_offset"s).AsArray()[1].AsDouble()};
    settings.stop_label_font_size_ = render_settings.at("stop_label_font_size"s).AsInt();
    settings.stop_label_offset_ = {render_settings.at("stop_label_offset"s).AsArray()[0].AsDouble(),
                                   render_settings.at("stop_label_offset"s).AsArray()[1].AsDouble()};
    settings.underlayer_color_ = ExtractColor(render_settings.at("underlayer_color"s));
    settings.underlayer_width_ = render_settings.at("underlayer_width"s).AsDouble();
    settings.color_palette_.reserve(render_settings.at("color_palette"s).AsArray().size());

    for (const auto &color: render_settings.at("color_palette"s).AsArray()) {
        settings.color_palette_.push_back(ExtractColor(color));
    }
}

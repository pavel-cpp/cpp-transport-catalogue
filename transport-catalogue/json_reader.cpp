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

void JsonReader::ProcessBaseRequests(TransportCatalogue &db) {
    using namespace std::literals;
    json::Node root = document_.GetRoot();
    if (root.AsMap().find("base_requests"s) == root.AsMap().end()) {
        return;
    }

    for (const auto &request: root.AsMap().at("base_requests"s).AsArray()) {
        if (request.AsMap().at("type"s) == "Stop"s) {
            db.AddStop(request.AsMap().at("name"s).AsString(), ExtractCoordinates(request.AsMap()));
        }
    }

    for (const auto &request: root.AsMap().at("base_requests"s).AsArray()) {
        if (request.AsMap().at("type"s) == "Stop"s) {
            std::string stopname = request.AsMap().at("name"s).AsString();
            for (const auto &[name, distance]: request.AsMap().at("road_distances"s).AsMap()) {
                db.AddDistance(stopname, name, distance.AsInt());
            }
        }
    }

    for (const auto &request: root.AsMap().at("base_requests"s).AsArray()) {
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
            }else if(stops.front() != stops.back()){
                stops.push_back(stops.front());
            }
            db.AddRoute(busname, stops);
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

void JsonReader::ProcessStatRequests(const TransportCatalogue &db, std::ostream &output) const {
    using namespace std::literals;
    json::Node root = document_.GetRoot();
    if (root.AsMap().find("stat_requests"s) == root.AsMap().end()) {
        return;
    }
    RequestHandler handler(db, renderer::MapRenderer());
    json::Array responses;
    for (const auto &request: root.AsMap().at("stat_requests"s).AsArray()) {
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

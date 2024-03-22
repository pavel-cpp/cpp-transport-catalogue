#include "json_reader.h"

#include <algorithm>
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
void JsonReader::ReadData(std::istream &input) {
    document_ = json::Load(input);
}

geo::Coordinates ExtractCoordinates(const json::Dict& dict) {
    return {dict.at("latitude").AsDouble(), dict.at("longitude").AsDouble()};
}

void JsonReader::ProcessBaseRequests(TransportCatalogue &db) {
    json::Node root = document_.GetRoot();
    if(root.AsMap().find("base_requests") != root.AsMap().end()) {
        return;
    }

    for(const auto& request: root.AsMap().at("base_requests").AsArray()) {
        if(request.AsMap().at("type") == "Stop") {
            std::string stopname = request.AsMap().at("name").AsString();
            db.AddStop(stopname, ExtractCoordinates(request.AsMap()));
            for(const auto& [name, distance]: request.AsMap().at("road_distances").AsMap()) {
                db.AddDistance(stopname, name, distance.AsInt());
            }
        }
    }

    for(const auto& request: root.AsMap().at("base_requests").AsArray()) {
        if(request.AsMap().at("type") == "Bus") {
            std::string busname = request.AsMap().at("name").AsString();
            std::vector<std::string_view> stops;
            for(const auto& stop: request.AsMap().at("stops").AsArray()) {
                stops.push_back(stop.AsString());
            }
            if(request.AsMap().at("is_roundtrip").AsBool()) {
                for(int i = stops.size() - 2; i >= 0; --i) {
                    stops.push_back(stops[i]);
                }
            }
            db.AddRoute(busname, stops);
        }
    }
}

json::Node AsJsonNode(const RouteInfo& route_info) {
    json::Dict dict;
    dict["route_length"] = route_info.length;
    dict["curvature"] = route_info.curvature;
    dict["stop_count"] = static_cast<int>(route_info.total_stops);
    dict["unique_stop_count"] = static_cast<int>(route_info.unique_stops);
    return dict;
}

void JsonReader::ProcessStatRequests(const TransportCatalogue &db, std::ostream &output) const {
    json::Node root = document_.GetRoot();
    if(root.AsMap().find("stat_requests") == root.AsMap().end()) {
        return;
    }
    RequestHandler handler(db, renderer::MapRenderer());
    json::Array responses;
    for (const auto& request: root.AsMap().at("stat_requests").AsArray()) {
        int request_id = request.AsMap().at("id").AsInt();
        json::Dict response;

        if(request.AsMap().at("type").AsString() == "Bus") {
            auto route_info = handler.GetBusStat(request.AsMap().at("name").AsString());
            if(route_info.has_value()) {
                response = AsJsonNode(*route_info).AsMap();
            }else {
                response["request_id"] = request_id;
                response["error_message"] = "not found";
            }
        }else if(request.AsMap().at("type").AsString() == "Stop") {
            try {
                auto buses = handler.GetBusesByStop(request.AsMap().at("name").AsString());
                response["buses"] = json::Array(buses.begin(), buses.end());
                response["request_id"] = request_id;
            }catch (std::out_of_range&) {
                response["request_id"] = request_id;
                response["error_message"] = "not found";
            }
        }
        responses.emplace_back(std::move(response));
    }
    json::Print(json::Document(std::move(responses)), output);
}

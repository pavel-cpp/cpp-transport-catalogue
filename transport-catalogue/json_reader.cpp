#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
void JsonReader::ReadData(std::istream &input) {
    document_ = json::Load(input);
}

void JsonReader::ProcessRequests(const TransportCatalogue &db) {
    json::Node root = document_.GetRoot();
    ProcessBaseRequests(db);
    if(root.AsMap().find("stat_requests") != root.AsMap().end()) {
        ProcessStatRequests(db);
    }
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
    for(const auto& request: root.AsMap().at("base_requests")) {
        if(request.at("type") == "Bus") {

        }
    }
}

void JsonReader::ProcessStatRequests(const TransportCatalogue &db, std::ostream &output) {

}

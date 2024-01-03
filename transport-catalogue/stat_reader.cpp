#include "stat_reader.h"

StatReader::Request StatReader::ParseRequest(std::string_view request) {
    int pos = request.find(' ');
    if (pos == std::string::npos) {
        return {};
    }
    return {
            std::string(request.substr(0, pos)),
            std::string(request.substr(pos + 1))
    };
}

void StatReader::ReadRequests(std::istream &input) {
    int stat_request_count;
    input >> stat_request_count >> std::ws;
    requests_.reserve(stat_request_count);
    for (int i = 0; i < stat_request_count; ++i) {
        std::string line;
        getline(input, line);
        requests_.push_back(ParseRequest(line));
    }
}

void StatReader::ExecuteAndPrintStat(const TransportCatalogue &transport_catalogue, std::ostream &output) {
    using std::operator ""s;
    using std::endl;

    for (const Request &request: requests_) {

        if (request.type == "Bus") {
            output << request.type << " "s << request.id << ": "s;
            try {
                auto route_info = transport_catalogue.BusRouteInfo(request.id);
                output.precision(6);
                output << route_info.total_stops
                       << " stops on route, "s
                       << route_info.unique_stops
                       << " unique stops, "s
                       << route_info.length
                       << " route length, "s
                       << route_info.curvature
                       << " curvature"s
                       << endl;
            } catch (std::out_of_range &) {
                output << "not found"s << endl;
            }
            continue;
        } else if (request.type == "Stop") {
            output << request.type << " "s << request.id << ": "s;
            try {
                auto buses = transport_catalogue.StopInfo(request.id);
                if (buses.empty()) {
                    output << "no buses"s << endl;
                    continue;
                }
                output << "buses"s;
                for (std::string_view bus: buses) {
                    output << ' ' << bus;
                }
                output << endl;
            } catch (std::out_of_range &) {
                output << "not found"s << endl;
            }
            continue;
        }

        throw std::invalid_argument("");
    }
}

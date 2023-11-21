// Local
#include "stat_reader.h"

Request ParseRequest(std::string_view request) {
    int pos = request.find(' ');
    if (pos == std::string::npos) {
        return {};
    }
    return {
            request.substr(0, pos),
            request.substr(pos + 1)
    };
}

void ParseAndPrintStat(const TransportCatalogue &transport_catalogue, std::string_view request,
                       std::ostream &output) {

    using std::operator ""s;
    using std::endl;
    Request parsed_request = ParseRequest(request);
    if (parsed_request.type == "Bus") {
        output << parsed_request.type << " "s << parsed_request.id << ": "s;
        try {
            auto route_info = transport_catalogue.BusRouteInfo(parsed_request.id);
            output.precision(6);
            output << route_info.total_stops
                   << " stops on route, "s
                   << route_info.unique_stops
                   << " unique stops, "s
                   << route_info.length
                   << " route length"s
                   << endl;
        }catch(std::out_of_range&){
            output << "not found"s << endl;
        }
        return;
    }else if(parsed_request.type == "Stop"){
        output << parsed_request.type << " "s << parsed_request.id << ": "s;
        try{
            auto buses = transport_catalogue.StopInfo(parsed_request.id);
            if(buses.empty()){
                output << "no buses"s << endl;
                return;
            }
            output << "buses"s;
            for(std::string_view bus: buses) {
                output << ' ' << bus;
            }
            output << endl;
        }catch(std::out_of_range&){
            output << "not found"s << endl;
        }
        return;
    }
    throw std::invalid_argument("");
}
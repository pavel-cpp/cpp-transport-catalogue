#pragma once

// IO
#include <iostream>

// Strings
#include <string_view>

// Local
#include "transport_catalogue.h"

class StatReader {
public:

    void ReadRequests(std::istream &input);

    void ExecuteAndPrintStat(const TransportCatalogue &transport_catalogue, std::ostream &output);

private:

    struct Request {
        explicit operator bool() const {
            return type.empty() && id.empty();
        }

        bool operator!() const {
            return !operator bool();
        }

        std::string type;
        std::string id;
    };

    std::vector<Request> requests_;

    Request ParseRequest(std::string_view request);

};
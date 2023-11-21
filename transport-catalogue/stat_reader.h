#pragma once

// IO
#include <iostream>

// Other
#include <string_view>

// Local
#include "transport_catalogue.h"


struct Request {
    explicit operator bool() const {
        return type.empty() && id.empty();
    }

    bool operator !() const {
        return !operator bool();
    }

    std::string_view type;
    std::string_view id;
};

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output);
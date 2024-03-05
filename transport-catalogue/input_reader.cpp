#include "input_reader.h"

// Other
#include <algorithm>
#include <iterator>
#include <regex>

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == std::string_view::npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);
    auto comma2 = str.find(',', not_space2);

    if (comma2 == std::string_view::npos) {
        comma2 = str.length();
    }

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2, comma2 - not_space)));

    return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == std::string_view::npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == std::string_view::npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != std::string_view::npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == std::string_view::npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

/*
 * Парсит рядом стоящие остановки вида (Di)m to (stop#)
 */
std::unordered_map<std::string_view, size_t> ParseNearestStops(std::string_view str) {
    std::unordered_map<std::string_view, size_t> near_stops;

    std::regex expr(R"((\d+)m to ([\w\s]*),*)");
    std::smatch matches;
    std::string temp_str_copy(str);

    while (std::regex_search(temp_str_copy, matches, expr)) {
        std::string stopname = matches[2].str();
        size_t shift = str.find(stopname);
        near_stops[std::string_view(str.begin() + shift, stopname.length())] = std::stoll(matches[1].str());
        temp_str_copy = matches.suffix();
    }

    return near_stops;
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands(TransportCatalogue &catalogue) const {

    for (const CommandDescription &cmd: commands_) {
        if (cmd.command == "Stop") {
            catalogue.AddStop(cmd.id, ParseCoordinates(cmd.description));
        }
    }

    for (const CommandDescription &cmd: commands_) {
        if (cmd.command == "Stop") {
            // TODO (Pavel): Тут добавлять расстояние между остановками
        }
    }

    for (const CommandDescription &cmd: commands_) {
        if (cmd.command == "Bus") {
            catalogue.AddRoute(cmd.id, ParseRoute(cmd.description));
        } else {
            throw std::invalid_argument("");
        }
    }
}

void InputReader::ReadCommands(std::istream &input) {
    int base_request_count;

    input >> base_request_count >> std::ws;

    for (int i = 0; i < base_request_count; ++i) {
        std::string line;
        getline(input, line);
        this->ParseLine(line);
    }
}

#pragma once

#include <istream>

#include "json.h"
#include "request_handler.h"
#include "transport_catalogue.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

class JsonReader {
public:
    JsonReader() = default;
    // Метод, который принимает на вход поток ввода в формате JSON и наполняет транспортный справочник
    void ReadData(std::istream &input);

    // Метод, который принимает на вход поток ввода в формате JSON и выполняет запросы к базе
    void ProcessBaseRequests(TransportCatalogue &db);

    void ProcessStatRequests(const TransportCatalogue &db, std::ostream &output) const;

private:
    json::Document document_{{}};
};

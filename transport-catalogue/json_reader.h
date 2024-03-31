#pragma once

#include <istream>

#include "json.h"
#include "request_handler.h"
#include "transport_catalogue.h"

class JsonReader final {
public:
    JsonReader() = default;
    // Метод, который принимает на вход поток ввода в формате JSON, и преобразует его в jsdon::Document
    void ReadData(std::istream &input);

    // Метод обработки Base запросов
    void ProcessBaseRequests(TransportCatalogue &db, renderer::MapRenderer& map) const;

    // Метод обработки Stat запросов
    void ProcessStatRequests(const RequestHandler &db, std::ostream &output) const;

    // Метод считывающий настройки
    [[nodiscard]] renderer::Settings GetRenderSettings() const;

private:
    json::Document document_{{}};
};

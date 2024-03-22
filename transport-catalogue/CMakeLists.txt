cmake_minimum_required(VERSION 3.23)
project(cpp-yandex-practicum)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        ${PROJECT_NAME}
        main.cpp
        transport_catalogue.cpp
        domain.cpp
        geo.cpp
        json.cpp
        json_reader.h
        json_reader.cpp
        map_renderer.h
        map_renderer.cpp
        request_handler.h
        request_handler.cpp
        svg.cpp
)

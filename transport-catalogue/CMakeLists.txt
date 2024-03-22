cmake_minimum_required(VERSION 3.23)
project(cpp-yandex-practicum)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        ${PROJECT_NAME}
        main.cpp
        input_reader.cpp
        stat_reader.cpp
        transport_catalogue.cpp
        domain.cpp
        geo.cpp
        json.cpp
        json_reader.cpp
        map_renderer.cpp
        request_handler.cpp
        svg.cpp
)

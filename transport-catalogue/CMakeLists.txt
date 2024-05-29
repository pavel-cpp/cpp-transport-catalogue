cmake_minimum_required(VERSION 3.23)
project(cpp-yandex-practicum CXX)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDebug)
endif ()

add_executable(
        ${PROJECT_NAME}
        main.cpp
        transport_catalogue.cpp
		transport_router.cpp
        domain.cpp
        geo.cpp
        json.cpp
        json_reader.cpp
        map_renderer.cpp
        request_handler.cpp
		json_builder.cpp
        svg.cpp
)

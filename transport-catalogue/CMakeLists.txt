set(CMAKE_MINIMUM_REQUIRED 3.23)
project(cpp-yandex-practicum)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        ${PROJECT_NAME}
        main.cpp
        input_reader.cpp
        stat_reader.cpp
        transport_catalogue.cpp
)

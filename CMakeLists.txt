cmake_minimum_required(VERSION 3.15)
project(http_server)

set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(http_server
    src/main.c
    src/server.c
    src/request.c
    src/router.c
    src/static_handler.c
)

target_link_libraries(http_server ws2_32)

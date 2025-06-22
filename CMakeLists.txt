cmake_minimum_required(VERSION 3.10)

project(HTTPServer VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(http_server
    main.c
    src/server.c
)

target_compile_options(http_server PRIVATE -Wall -Wextra -pedantic)

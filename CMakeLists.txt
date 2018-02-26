cmake_minimum_required(VERSION 3.5.1)

set(BIN catalog)

project(${BIN})
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-Wall")

include_directories(include)

add_executable(${BIN} src/main.c src/catalog.c)
target_link_libraries(${BIN})

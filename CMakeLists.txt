cmake_minimum_required(VERSION 3.5.1)

include_directories(include)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-Wall")

project(basic)
add_executable(basic src/catalog.c examples/basic/basic.c)

project(opengl)
add_executable(opengl src/catalog.c examples/opengl/opengl.c)
target_link_libraries(opengl m SDL2 GL GLEW)

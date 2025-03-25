cmake_minimum_required(VERSION 3.23)
project(3d_renderer LANGUAGES CXX)

add_subdirectory(lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

file(GLOB_RECURSE APPLICATION_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/application/*.cpp)

add_executable(3d_renderer ${APPLICATION_SOURCES})

target_include_directories(3d_renderer PUBLIC lib/include)

find_package(SFML REQUIRED COMPONENTS Graphics)
target_link_libraries(3d_renderer PUBLIC SFML::Graphics 3d_renderer_lib)

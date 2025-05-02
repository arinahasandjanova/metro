cmake_minimum_required(VERSION 3.29)
project(metro)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
add_library(metro_lib all_stations.cpp station.cpp train.cpp clock.cpp)
target_include_directories(metro_lib PUBLIC ${CMAKE_SOURCE_DIR})

add_executable(metro main.cpp)

target_link_libraries(metro PRIVATE metro_lib)

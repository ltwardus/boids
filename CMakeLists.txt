cmake_minimum_required(VERSION 3.2)
project(Boids)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2 COMPONENTS system graphics window REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

add_executable(boids src/main.cc)
target_link_libraries(boids ${SFML_LIBRARIES})

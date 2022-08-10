cmake_minimum_required(VERSION 3.22.1)

project(DisplayImage)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(DisplayImage Neighborhood.cpp PointVertex.cpp PointGraph.cpp ImageOps.cpp DisplayImage.cpp)

target_link_libraries(DisplayImage ${OpenCV_LIBS})

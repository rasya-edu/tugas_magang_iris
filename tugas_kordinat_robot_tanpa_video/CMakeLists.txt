cmake_minimum_required(VERSION 3.5)
project(MyOpenCVProject)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(MyOpenCVProject main.cpp)
target_link_libraries(MyOpenCVProject ${OpenCV_LIBS})

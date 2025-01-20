cmake_minimum_required(VERSION 3.20)
set(MBED_PATH ${CMAKE_SOURCE_DIR}/mbed-os CACHE INTERNAL "")

include(${MBED_PATH}/tools/cmake/app.cmake)

project(sx1276_project)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} mbed-os)


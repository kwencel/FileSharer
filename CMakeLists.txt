cmake_minimum_required(VERSION 3.6)
project (FileSharer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Boost COMPONENTS program_options filesystem REQUIRED)

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

add_subdirectory(common)
include_directories(${COMMON_INCLUDE_DIRS})
add_subdirectory(client)
add_subdirectory(tracker)
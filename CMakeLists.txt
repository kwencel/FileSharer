cmake_minimum_required(VERSION 3.6)
project (FileSharer)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(common)
include_directories(${COMMON_INCLUDE_DIRS})
add_subdirectory(client)
add_subdirectory(tracker)
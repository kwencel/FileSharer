cmake_minimum_required(VERSION 3.6)
project(Tracker)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(SOURCE_FILES
        src/Tracker.cpp
        include/TrackerHeader.h)

include_directories(include)

add_executable(tracker ${SOURCE_FILES})

target_link_libraries(tracker ${Boost_LIBRARIES})
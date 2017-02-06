cmake_minimum_required(VERSION 3.6)
project(Client)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(SOURCE_FILES
        src/Client.cpp
        include/ClientHeader.h)

include_directories(include)

add_executable(client ${SOURCE_FILES})

target_link_libraries(client ${Boost_LIBRARIES})
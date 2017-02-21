cmake_minimum_required(VERSION 3.6)
project(Tracker)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

file(GLOB SOURCE_FILES "src/*.cpp")
file(GLOB HEADER_FILES "include/*.h")

include_directories(include)

add_executable(tracker ${SOURCE_FILES} ${HEADER_FILES} ${COMMON_SOURCE_FILES} ${COMMON_HEADER_FILES})

target_link_libraries(tracker ${Boost_LIBRARIES} Threads::Threads)
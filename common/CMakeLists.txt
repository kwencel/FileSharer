cmake_minimum_required(VERSION 3.6)
project(Common)

file(GLOB SOURCE_FILES "src/*.cpp")
file(GLOB HEADER_FILES "include/*.h")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(COMMON_SOURCE_FILES ${SOURCE_FILES} CACHE INTERNAL "" FORCE)
set(COMMON_HEADER_FILES ${HEADER_FILES} CACHE INTERNAL "" FORCE)

set(COMMON_INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/logger/easyloggingpp/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/util"

        CACHE INTERNAL "" FORCE
        )

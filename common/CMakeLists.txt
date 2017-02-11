cmake_minimum_required(VERSION 3.6)
project(Common)

set(COMMON_INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/logger/easyloggingpp/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/util"
        "${CMAKE_CURRENT_SOURCE_DIR}/config"

        CACHE INTERNAL "" FORCE
        )

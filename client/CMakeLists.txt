cmake_minimum_required(VERSION 3.6)
project(Client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

file(GLOB SOURCE_FILES "src/*.cpp")
file(GLOB HEADER_FILES "include/*.h")

include_directories(include)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

############### QT ###############
set(QT_BIN "./qt_gui/build")
set(QT_SRC "./qt_gui/GUI")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

file(GLOB QT_HEADERS_FILES ${QT_SRC}/*.h)
file(GLOB QT_SOURCE_FILES ${QT_SRC}/*.cpp)
include_directories(${QT_SRC})

add_executable(client ${SOURCE_FILES} ${HEADER_FILES} ${COMMON_SOURCE_FILES} ${COMMON_HEADER_FILES}
               ${QT_SOURCE_FILES} ${QT_HEADER_FILES} ${QT_GEN_SOURCE} ${QT_GEN_HEADERS})

target_link_libraries(client ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} Threads::Threads Qt5::Core Qt5::Widgets Qt5::Gui)
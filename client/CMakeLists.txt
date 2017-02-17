cmake_minimum_required(VERSION 3.6)
project(Client)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

file(GLOB SOURCE_FILES "src/*.cpp")
file(GLOB HEADER_FILES "include/*.h")

include_directories(include)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(client ${SOURCE_FILES} ${HEADER_FILES} ${COMMON_SOURCE_FILES} ${COMMON_HEADER_FILES})
add_executable(test_client test_client/TestClient.cpp include/Connection.h src/Connection.cpp
               src/ConnectionManager.cpp include/ConnectionManager.h)

target_link_libraries(client ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} Threads::Threads)
target_link_libraries(test_client ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} Threads::Threads)
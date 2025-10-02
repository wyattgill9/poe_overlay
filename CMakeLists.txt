cmake_minimum_required(VERSION 3.31)

project(poe_tracker VERSION 1.0 LANGUAGES CXX)

set(SOURCES
    src/passive_tree.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++23")

add_library(poe_tracker ${SOURCES})

target_include_directories(poe_tracker
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

add_executable(main app/main.cpp)
target_link_libraries(main PRIVATE poe_tracker)

set_target_properties(poe_tracker PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

find_package(OpenSSL REQUIRED)
target_link_libraries(poe_tracker PUBLIC OpenSSL::SSL OpenSSL::Crypto)

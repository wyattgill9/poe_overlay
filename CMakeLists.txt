cmake_minimum_required(VERSION 3.31)

project(poe_tracker VERSION 1.0 LANGUAGES CXX)

# prevent macos sdk headers from being auto pulled in
set(CMAKE_OSX_SYSROOT "" CACHE PATH "SDK path" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "" CACHE STRING "Deployment target" FORCE)

# force compiler to use clang
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# shitty fix, tells cmake to just trust me but
# TODO: fix this cause this makes clangd break asf
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(SOURCES
    src/passive_tree.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

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

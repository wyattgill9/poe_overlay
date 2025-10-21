cmake_minimum_required(VERSION 3.31)

project(poe_overlay VERSION 1.0 LANGUAGES CXX)

set(CMAKE_OSX_DEPLOYMENT_TARGET "13.3" CACHE STRING "Minimum OS X deployment version")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(SOURCES
    src/passive_tree.cpp
    src/poe2_client.cpp
    src/poe_logger.cpp
)

add_library(poe_overlay ${SOURCES})

target_include_directories(poe_overlay
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external
)

target_compile_features(poe_overlay PUBLIC cxx_std_23)

# HTTPS
find_package(OpenSSL REQUIRED)
target_link_libraries(poe_overlay PUBLIC OpenSSL::SSL OpenSSL::Crypto)

# Containers
# find_package(absl CONFIG REQUIRED)
# target_link_libraries(poe_overlay PRIVATE absl::flat_hash_map)

# find_package(Boost REQUIRED)
# target_link_libraries(poe_overlay PUBLIC Boost::headers)

add_executable(main app/main.cpp)
target_link_libraries(main PRIVATE poe_overlay)
target_compile_features(main PRIVATE cxx_std_23)

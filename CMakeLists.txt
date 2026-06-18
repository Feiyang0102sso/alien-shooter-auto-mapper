cmake_minimum_required(VERSION 3.20)
project(auto_mapper VERSION 0.1.0 LANGUAGES CXX)

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MINGW)
    add_link_options(-static)
    # Strip symbols and optimize in Release mode
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3)
        add_link_options(-Wl,-s)
    endif()
endif()

# set output dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# set src as root, so, #include "auto_mapper/common/logger.h"
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# common/
set(COMMON_SOURCES
    src/auto_mapper/common/config.cpp
    src/auto_mapper/common/logger.cpp
)

# static libs
add_library(auto_mapper_common STATIC ${COMMON_SOURCES})

# main
add_executable(manual_test tests/manual_test.cpp)
target_link_libraries(manual_test PRIVATE auto_mapper_common)

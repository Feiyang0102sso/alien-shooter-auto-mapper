cmake_minimum_required(VERSION 3.20)
project(auto_mapper VERSION 0.1.0 LANGUAGES CXX)

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# set src as root, so, #include "auto_mapper/common/logger.h"
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.13.0
)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(spdlog googletest)

enable_testing()

# common/
set(COMMON_SOURCES
    src/auto_mapper/common/config.cpp
    src/auto_mapper/common/logger.cpp
)

# static libs
add_library(auto_mapper_common STATIC ${COMMON_SOURCES})
target_link_libraries(auto_mapper_common PUBLIC spdlog::spdlog)

# io/
set(IO_SOURCES
    src/auto_mapper/io/map_writer.cpp
)
add_library(auto_mapper_io STATIC ${IO_SOURCES})
target_link_libraries(auto_mapper_io PUBLIC auto_mapper_common)

# core/
set(CORE_SOURCES
    src/auto_mapper/core/geometry.cpp
    src/auto_mapper/core/wall_builder.cpp
    src/auto_mapper/core/door_builder.cpp
    src/auto_mapper/core/dir_randomizer.cpp
)
add_library(auto_mapper_core STATIC ${CORE_SOURCES})
target_link_libraries(auto_mapper_core PUBLIC auto_mapper_common auto_mapper_io)

if(WIN32)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/auto_mapper/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY
    )
    set(RESOURCE_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
endif()

# main shared library (DLL)
add_library(auto_mapper SHARED src/auto_mapper/api.cpp ${RESOURCE_SOURCES})
target_compile_definitions(auto_mapper PRIVATE AUTO_MAPPER_EXPORTS)
target_link_libraries(auto_mapper PRIVATE auto_mapper_core auto_mapper_io auto_mapper_common)
if(MINGW)
    target_link_options(auto_mapper PRIVATE -static -static-libgcc -static-libstdc++)
endif()

add_custom_command(TARGET auto_mapper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:auto_mapper>
        $<TARGET_FILE_DIR:auto_mapper>/AutoMapper.dll
    COMMENT "Copying build output to AutoMapper.dll"
)


# tests
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "tests/test_*.cpp")

add_executable(auto_mapper_tests ${TEST_SOURCES})
target_link_libraries(auto_mapper_tests PRIVATE
    auto_mapper_core 
    auto_mapper_io 
    auto_mapper_common 
    GTest::gtest_main
)

add_dependencies(auto_mapper_tests auto_mapper) # build dll every time when running test

add_test(NAME AllTests COMMAND auto_mapper_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

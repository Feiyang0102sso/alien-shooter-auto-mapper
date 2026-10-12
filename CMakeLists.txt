# copy_directory_if_different needs CMake 3.26.
cmake_minimum_required(VERSION 3.26)
# The application version lives only in src/editor/editor.rc.
project(auto_mapper LANGUAGES CXX)

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Only the MSVC Release toolchain is supported.
if(NOT MSVC)
    message(FATAL_ERROR "AutoMapper builds with MSVC only.")
endif()
# Keep UTF-8 source text and the CRT consistent across the editor and tests.
add_compile_options(/utf-8)
set(gtest_force_shared_crt ON CACHE BOOL "Use the shared MSVC runtime" FORCE)
# Bundled fmt 9 uses stdext iterators removed by recent MSVC releases.
set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "Use C++20 formatting with MSVC" FORCE)


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

# Qt SDK location comes from CMAKE_PREFIX_PATH in CMakePresets.json.
find_package(Qt6 6.11.1 REQUIRED COMPONENTS Widgets Concurrent Test)
# Qt bin folder: windeployqt for the editor, DLL search path for the UI tests.
get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

enable_testing()

# Core algorithms            ->  auto_mapper_core (static)
add_subdirectory(src/auto_mapper)
# Qt editor                  ->  AutoMapper.exe
add_subdirectory(src/editor)
# Core tests, editor tests and the golden candidate exporter
add_subdirectory(tests)

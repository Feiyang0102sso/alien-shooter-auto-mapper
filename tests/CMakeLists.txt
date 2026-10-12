# Test executables land next to AutoMapper.exe so they share its Qt DLLs and assets/ folder.

# Core tests (GoogleTest)
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")

add_executable(auto_mapper_tests ${TEST_SOURCES})
set_target_properties(auto_mapper_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
target_link_libraries(auto_mapper_tests PRIVATE
    auto_mapper_core
    GTest::gtest_main
)

add_test(NAME AllTests COMMAND auto_mapper_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# Editor tests (Qt Test), run offscreen; detailed results go to native-editor-results.txt.
add_executable(auto_mapper_editor_tests editor/test_editor.cpp)
set_target_properties(auto_mapper_editor_tests PROPERTIES AUTOMOC ON RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
target_link_libraries(auto_mapper_editor_tests PRIVATE auto_mapper_editor_ui Qt6::Test)
# The editor build step syncs assets/ and deploys the Qt runtime the tests rely on.
add_dependencies(auto_mapper_editor_tests auto_mapper_editor)
target_compile_definitions(auto_mapper_editor_tests PRIVATE AUTO_MAPPER_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
add_test(NAME NativeEditor COMMAND auto_mapper_editor_tests -platform offscreen
    -o "${CMAKE_BINARY_DIR}/native-editor-results.txt,txt")
set_tests_properties(NativeEditor PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    ENVIRONMENT "QT_PLUGIN_PATH=${QT_BIN_DIR}/../plugins"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${QT_BIN_DIR}")

# Explicit candidate export keeps golden updates separate from normal tests.
add_executable(auto_mapper_golden_candidates EXCLUDE_FROM_ALL "${CMAKE_SOURCE_DIR}/tools/golden_candidates.cpp")
target_include_directories(auto_mapper_golden_candidates PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(auto_mapper_golden_candidates PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
target_link_libraries(auto_mapper_golden_candidates PRIVATE auto_mapper_core)

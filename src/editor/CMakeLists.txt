# Qt editor: UI library (shared with the editor tests), AutoMapper.exe, runtime deployment and packaging.
set(CMAKE_AUTOMOC ON)

# Layout follows the former Python editor package: config/logger, i18n, catalog, project, hotkeys, ui/.
add_library(auto_mapper_editor_ui STATIC
    config.h config.cpp
    logger.h logger.cpp
    i18n/locale.h i18n/locale.cpp i18n/text_keys.h
    catalog/catalog.h catalog/catalog.cpp
    project/project.h project/project.cpp
    hotkeys/hotkey_config.h hotkeys/hotkey_config.cpp
    hotkeys/hotkey_manager.h hotkeys/hotkey_manager.cpp
    hotkeys/hotkey_dialog.h hotkeys/hotkey_dialog.cpp
    ui/colors.h ui/colors.cpp
    ui/widget_helpers.h
    ui/main_window.h ui/main_window.cpp
    ui/canvas/viewport.h ui/canvas/viewport.cpp
    ui/panels/as1_ceiling.h ui/panels/as1_ceiling.cpp
    ui/panels/inspector.h ui/panels/inspector.cpp
    ui/panels/left_shelf.h ui/panels/left_shelf.cpp
    ui/panels/theme_shelf.h ui/panels/theme_shelf.cpp
    ui/tools/drawing_modes.h
    ui/tools/drawing_toolbar.h ui/tools/drawing_toolbar.cpp
    ui/tools/eraser/eraser_geometry.h ui/tools/eraser/eraser_geometry.cpp
    ui/tools/eraser/eraser_properties.h ui/tools/eraser/eraser_properties.cpp
)
target_link_libraries(auto_mapper_editor_ui PUBLIC auto_mapper_core Qt6::Widgets Qt6::Concurrent)
# Keep file/line in Release Qt messages so AutoMapper.log can point at the source.
target_compile_definitions(auto_mapper_editor_ui PUBLIC QT_MESSAGELOGCONTEXT)

# Small text resources are embedded: :/catalog.json, :/i18n/*.json, :/styles/*.qss.
# Preview images stay outside the exe in <exe dir>/assets/preview.
file(GLOB_RECURSE EDITOR_TEXT_RESOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.json"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.qss"
)
qt_add_resources(auto_mapper_editor_ui editor_assets
    PREFIX "/" BASE "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    FILES ${EDITOR_TEXT_RESOURCES}
)

# editor.rc holds the icon and the application version.
qt_add_executable(auto_mapper_editor WIN32 main.cpp editor.rc)
set_target_properties(auto_mapper_editor PROPERTIES OUTPUT_NAME AutoMapper RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
target_link_libraries(auto_mapper_editor PRIVATE auto_mapper_editor_ui)
# Keep build/<preset>/assets/preview in sync so the exe runs straight from the build directory.
add_custom_command(TARGET auto_mapper_editor POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/assets/preview" "$<TARGET_FILE_DIR:auto_mapper_editor>/assets/preview"
    COMMENT "Syncing preview images into the build directory"
)

# Copy the Qt DLLs and plugins next to the exe so it runs straight from the build directory.
set(QT_WINDEPLOYQT "${QT_BIN_DIR}/windeployqt.exe")
add_custom_command(TARGET auto_mapper_editor POST_BUILD
    COMMAND "${QT_WINDEPLOYQT}" --release
        --no-translations --no-opengl-sw --no-system-d3d-compiler "$<TARGET_FILE:auto_mapper_editor>"
    COMMENT "Deploying the native editor Qt runtime"
)

# Portable package (pack_release.bat): exe, assets/, Qt runtime and app-local CRT.
# Only files something actually loads are shipped (checked with dumpbin /dependents):
#   Qt6Core/Gui/Widgets, plugins platforms/qwindows, imageformats/qwebp (previews),
#   styles/qmodernwindowsstyle (the "windows11" style), qt.conf (points Qt at plugins/).
install(TARGETS auto_mapper_editor RUNTIME DESTINATION . COMPONENT NativeEditor)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/preview" DESTINATION assets COMPONENT NativeEditor)
install(CODE "set(QT_DEPLOY_BIN_DIR .)" COMPONENT NativeEditor)
qt_generate_deploy_app_script(TARGET auto_mapper_editor OUTPUT_SCRIPT EDITOR_DEPLOY_SCRIPT NO_TRANSLATIONS
    DEPLOY_TOOL_OPTIONS --no-compiler-runtime --no-system-d3d-compiler --no-system-dxc-compiler
    --no-opengl-sw --skip-plugin-types generic,networkinformation,tls
    --exclude-plugins qgif,qicns,qico,qjpeg,qtga,qtiff,qwbmp)
install(SCRIPT ${EDITOR_DEPLOY_SCRIPT} COMPONENT NativeEditor)

# App-local release CRT avoids bundling the much larger VC++ installer.
# CMake would copy the whole CRT set; install only the DLLs the exe, Qt and its plugins import.
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)
set(EDITOR_REQUIRED_CRT msvcp140.dll msvcp140_1.dll msvcp140_2.dll vcruntime140.dll vcruntime140_1.dll)
set(EDITOR_CRT_FILES "")
foreach(crt_file IN LISTS CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
    get_filename_component(crt_name "${crt_file}" NAME)
    string(TOLOWER "${crt_name}" crt_name)
    if(crt_name IN_LIST EDITOR_REQUIRED_CRT)
        list(APPEND EDITOR_CRT_FILES "${crt_file}")
    endif()
endforeach()
list(LENGTH EDITOR_CRT_FILES crt_count)
list(LENGTH EDITOR_REQUIRED_CRT required_count)
if(NOT crt_count EQUAL required_count)
    message(FATAL_ERROR "Missing VC++ runtime DLLs for packaging. Found: ${EDITOR_CRT_FILES}")
endif()
install(FILES ${EDITOR_CRT_FILES} DESTINATION . COMPONENT NativeEditor)

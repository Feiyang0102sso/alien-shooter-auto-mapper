# Core algorithms: common -> io -> core, all static and linked into AutoMapper.exe.
# The editor calls core::generate_map() (core/map_generator.h) directly.

# common/
set(COMMON_SOURCES
    common/config.cpp
    common/logger.cpp
)

# static libs
add_library(auto_mapper_common STATIC ${COMMON_SOURCES})
target_link_libraries(auto_mapper_common PUBLIC spdlog::spdlog)

# io/
set(IO_SOURCES
    io/as1_map_writer.cpp
    io/as2_map_writer.cpp
    io/map_writer.cpp
)
add_library(auto_mapper_io STATIC ${IO_SOURCES})
target_link_libraries(auto_mapper_io PUBLIC auto_mapper_common)

# core/
set(CORE_SOURCES
    core/geometry.cpp
    core/wall_builder/wall_builder.cpp
    core/door_builder/door_builder.cpp
    core/decoration_builder/decoration_builder.cpp
    core/decoration_builder/decoration_library.cpp
    core/decoration_builder/placeholder_vids.cpp
    core/randomizer.cpp
    core/indoor_decorations/array_layout.cpp
    core/indoor_decorations/desk_builder.cpp
    core/indoor_decorations/incubator_builder.cpp
    core/map_generator.cpp
)
add_library(auto_mapper_core STATIC ${CORE_SOURCES})
target_link_libraries(auto_mapper_core PUBLIC auto_mapper_common auto_mapper_io)

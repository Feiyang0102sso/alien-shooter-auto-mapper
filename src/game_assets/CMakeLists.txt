# Read-only access to installed game data (objects.res, Vid/*.vid): RES container, OBJ table, single-frame
# AS1 / AS2 vid decoding. Parked code: not added by the root CMakeLists.txt and not used by AutoMapper.exe.
# To build it again: add_subdirectory(src/game_assets) after src/auto_mapper (it needs auto_mapper_common).
add_library(auto_mapper_game_assets STATIC
    game_family.h
    res/byte_reader.h
    res/res_file.h res/res_file.cpp
    objects/object_table.h objects/object_table.cpp
    vid/vid_file.h vid/vid_file.cpp
    vid/vid_library.h vid/vid_library.cpp
)
target_link_libraries(auto_mapper_game_assets PUBLIC auto_mapper_common)

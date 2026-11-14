# Auto Mapper v2.0.0

**English** | [中文](README_CN.md)

Auto Mapper is an automatic map layout tool for Alien Shooter. You draw walls, place decorations, and brush in loot boxes and corpses on a canvas; it turns them into objects in the game's `.map` file, ready for you to verify and refine in the map editor.

## Features

- **Walls**: draw walls on the grid with polyline, straight line, and rectangle tools; walls, doors, floors, and ceilings are laid out automatically.
- **Curve walls** (AS2 series): drag freely to fill a curve with rock, alien nest, or cliff pieces; enclosed areas can get a matching floor automatically.
- **Decoration sets**: 21 ready-made indoor sets for the AS2 series and 10 for AS1; pick one and click on the canvas to place it.
- **Loot boxes** (AS2 series): wood box, metal box, and alien egg brushes that drop a whole pile per stroke; loot tables are editable and can be saved as reusable presets.
- **Gore** (AS2 series): bone pile, monster corpse, soldier corpse, and blood pool brushes, with blood scattered around each corpse.
- **Sprite preview**: once a game folder is set, the canvas shows decorations, loot boxes, and corpses with the game's own sprites.
- **Project files**: projects are saved as JSON and can be imported later to keep editing.
- **Game versions**: map formats and wall sets for AS1 and the AS2 series (AS2, AS2R/C, AS2OE).
- **Interface**: English / Chinese, with keyboard shortcuts for common actions.

## Important warning

> [!WARNING]
> Auto Mapper's algorithms cannot be guaranteed to be perfect in every map or
> layout. Always treat the generated result as a starting point and verify it
> in your map editor. The map editor is the final authority.

A few more limitations of the preview:

- The canvas shows game sprites only after a game folder (the one containing `objects.res` and `Vid`) is set in the **Sprite Preview** (F9) window;
  otherwise objects are marked with green crosses. The AS1 and AS2 folders are set separately.
- The sprite draw order is only an approximation; trust what the map editor shows.
- Walls, doors, and floors are drawn as outlines, not sprites.
- Loot tables are rolled again every time a map is generated, so two exports of the same project will not drop exactly the same items.

## Recommended workflow

Auto Mapper currently cannot edit an existing map. The recommended workflow is:

1. Create or prepare the map in Auto Mapper.
2. Finish and verify all automatic parts first: walls, doors, floors, and ceilings.
3. Open the generated map in your map editor and confirm that the automatic layout is correct before starting extensive manual editing.
4. For decorations especially, adjust a small part at a time and verify it in the map editor as you go.

## Getting started

1. Unzip the release package and run `AutoMapper.exe`. On first launch you will be asked to choose the interface language.
2. Choose the game on the left (AS1 / AS2 series and its version) and set the map size in the right panel. Switching between AS1 and the AS2 series starts a new project.
3. Pick a shelf on the left: **Wall Sets**, **Decoration Sets**, and, in AS2 series projects, **Loot Boxes** and **Gore**.
4. Draw on the canvas and tune the current tool in the right panel. F1 clears the canvas to start over.
5. **Generate Map** (F4) writes the `.map` file; **Export JSON** (F3) saves the project so you can keep editing later.

The `assets/` folder next to `AutoMapper.exe` holds the decoration library maps and previews. Do not delete it.

## Keyboard shortcuts

| Group | Key | Action |
|---|---|---|
| Functions | F1 / F2 / F3 | New / Import JSON / Export JSON |
| | F4 | Generate Map |
| | F5 / F6 | Toggle Floor / Ceiling |
| | F7 / F8 | Door State / Random Direction |
| | F9 | Sprite Preview |
| Set navigation | Q / E | Previous / Next Set |
| | - / + | Previous / Next item in the set |
| Tools | 0 | Select Decorations |
| | 1 / 2 / 3 | Polyline / Straight Line / Rectangle |
| | 4 | Eraser |
| | 5 | Freehand (curve walls, loot boxes, gore) |
| | Del | Delete Selected Decoration |

The **Shortcuts** button in the app shows the same table. Shortcuts are ignored while an input field has focus.

## Building from source

Requires Visual Studio 2026 (MSVC v145) and Qt 6.11.1 (Qt Base, Qt Image Formats, x64). No CMake needed.

- Open `vs/AutoMapper.sln` in Visual Studio and build. The Qt path defaults to `vs/qt.props` and can be overridden with the `QTDIR` environment variable.
- Or run `builder.bat` to build Release from the command line and run all tests.
- After building, run `pack_release.bat` to create the release folder and zip.

Output goes to `bin/x64/<Configuration>/`.

## Changelog

- **v2.0.0 (2026-11-13):**
  - Reworked decoration sets: previews are rendered directly from the game's Vid files, and AS1 decoration sets are now supported.
  - AS1 / AS2 game folders can be added to the sprite preview settings.
  - Added curve walls (AS2 series): freehand rock, alien nest, and cliff, with matching floors laid automatically.
  - Added the Loot Boxes shelf (AS2 series): wood box, metal box, and alien egg brushes, with loot tables and loot presets (`loot_presets.ini`).
  - Added the Gore shelf (AS2 series): bone pile, monster corpse, soldier corpse, and blood pool brushes.
- **v1.1.0 (2026-10-12):**
  - Rewrote the UI in C++ / Qt.
- **v1.0 (2026-09-06):**
  - Added the ceiling generation algorithm.
  - Added decoration presets.
  - Added keyboard shortcuts.
  - Added support for the three AS2 variants: AS2, AS2R/C, and AS2OE.
- **v0.1.0 (2026-08-11):**
  - Initial pre-release.

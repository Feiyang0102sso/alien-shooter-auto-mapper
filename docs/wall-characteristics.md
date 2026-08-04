# Wall Characteristics

This document defines the shared wall terms used by AS1 and AS2 wall analysis.

## Direction Names

`dir_a` is the canonical `/` wall direction.

`dir_b` is the canonical `\` wall direction.

`pillar` is the endpoint, corner, or intersection part for a wall set. A pillar does not define the wall step by itself because it usually appears at isolated vertices.

These names follow `WallProfile` in the C++ core. They should be treated as domain names even when a specific game asset has visual variants.

## Coordinate Model

Wall placement uses three coordinate spaces:

- Grid coordinate: logical wall grid point `(gx, gy)`.
- Map coordinate: physical `.map` coordinate `(posX, posY)`.
- Screen coordinate: editor pixel coordinate after zoom, pan, and viewport transform.

Wall step inference must use Map coordinates from sprite records, not Screen coordinates from screenshots.

The core isometric projection is:

```text
posX = (gx - gy) * step_x + shift_x
posY = (gx + gy) * step_y + shift_y
```

For two sprites in the same wall set, the shift cancels out:

```text
dx = posX2 - posX1
dy = posY2 - posY1
```

## Direction Deltas

For adjacent wall grid points:

```text
dir_a (/) moves along Grid +Y:
dx = -step_x
dy = +step_y

dir_b (\) moves along Grid +X:
dx = +step_x
dy = +step_y
```

When the two sampled sprites are read in reverse order, both signs are reversed. Inference tools should normalize samples to positive `dy` before classifying the axis.

After normalization:

```text
dir_a (/): dx < 0, dy > 0
dir_b (\): dx > 0, dy > 0
step_x = abs(dx)
step_y = abs(dy)
```

## Wall Offsets

`offset_a`, `offset_b`, and `offset_p` are per-part Map coordinate adjustments applied after the base Grid-to-Map projection.

They are not wall steps. A wall step describes spacing between logical wall grid points. An offset describes where one sprite asset should be anchored around its projected grid point.

The C++ placement order is:

```text
base_pos = project Grid coordinate with step_x and step_y
final_pos = base_pos + part offset
```

For inference, use this convention unless a wall set proves otherwise:

```text
pillar sprites are grid anchors
offset_p = (0, 0)
```

Then direction offsets can be estimated from nearby pillar anchors:

```text
dir_a theoretical point = pillar_pos + n * (-step_x, +step_y)
offset_a = dir_a_sprite_pos - dir_a theoretical point

dir_b theoretical point = pillar_pos + n * (+step_x, +step_y)
offset_b = dir_b_sprite_pos - dir_b theoretical point
```

`n` is a positive wall-grid distance from the pillar. Repeated offsets across the calibration room are stronger candidates.

## Grid Difference Formula

When `step_x` and `step_y` are known, a Map coordinate delta can be converted back to a Grid delta:

```text
gx_diff = ((dx / step_x) + (dy / step_y)) / 2
gy_diff = ((dy / step_y) - (dx / step_x)) / 2
```

Expected adjacent wall results:

```text
dir_a (/): gx_diff = 0, gy_diff = 1
dir_b (\): gx_diff = 1, gy_diff = 0
```

## 3x3 Calibration Room

A 3x3 wall sample can infer the shared wall step when each straight wall VID appears in repeated adjacent positions.

Recommended sample content:

- at least three repeated `dir_a` wall sprites
- at least three repeated `dir_b` wall sprites
- enough corner or endpoint sprites to identify likely pillars

The sample does not need every visual variant to be a full 3x3 room. Variants can be placed as short repeated strips after the baseline room.

## AS2 Wall Set 1 Example

The current `wall_set1` sample has these inferred features:

```text
step_x = 90
step_y = 64
dir_a-like VID = 1781
dir_b-like VID = 1780
pillar-like VID = 1782
```

The important repeated deltas are:

```text
1781: (-90, +64)
1780: (+90, +64)
```

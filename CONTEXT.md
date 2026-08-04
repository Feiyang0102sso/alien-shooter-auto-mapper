# Project Glossary

## Map Format

Alien Shooter map file variant supported by the writer and binary templates.

## AS1

Alien Shooter 1 / Zombie Shooter 1 map format.

## AS2

Alien Shooter 2 legacy map format. Its empty map template uses map version `0x13`.

## AS2R

Unified name for Alien Shooter 2 Reloaded, Alien Shooter 2 Conscription, and Zombie Shooter 2 compatible map format. Its empty map template uses map version `0x14`.

## Wall Set

A group of wall sprites that share the same physical grid step and visual style.

## Grid Coordinate

The logical wall grid coordinate used by the editor and generator, written as `(gx, gy)`.

Grid coordinates describe wall topology: which logical wall points are connected.

## Map Coordinate

The physical coordinate stored in a game `.map` sprite record, written as `(posX, posY)`.

Map coordinates are the final coordinates consumed by the game.

## Screen Coordinate

The editor canvas pixel coordinate after zoom, pan, centering, and viewport transforms.

Screen coordinates are for interaction and rendering only. Do not use them to infer wall steps.

## Wall Step

The physical coordinate delta used to project adjacent wall grid points into map coordinates.

Core projection:

```text
posX = (gx - gy) * step_x + shift_x
posY = (gx + gy) * step_y + shift_y
```

For same-wall-set sprite deltas, `shift_x` and `shift_y` cancel out.

## Wall Offset

The per-part Map coordinate adjustment applied after Grid coordinates are projected with `step_x` and `step_y`.

Offsets align sprite anchors to the intended visual wall point. They do not change wall step.

## Direction A

The canonical `/` wall direction in a wall set.

Normalized adjacent delta:

```text
dx = -step_x
dy = +step_y
```

## Direction B

The canonical `\` wall direction in a wall set.

Normalized adjacent delta:

```text
dx = +step_x
dy = +step_y
```

## Pillar

A wall set sprite used at endpoints, corners, and wall intersections.

## Wall Step Inference

Infer wall steps from normalized adjacent same-VID deltas:

```text
step_x = abs(dx)
step_y = abs(dy)
```

Convert Map delta back to Grid delta when `step_x` and `step_y` are known:

```text
gx_diff = ((dx / step_x) + (dy / step_y)) / 2
gy_diff = ((dy / step_y) - (dx / step_x)) / 2
```

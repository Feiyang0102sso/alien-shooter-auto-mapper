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

## Wall Variant

A visual asset choice inside a wall set. A wall set may have different variant counts for direction parts and pillar parts.

Wall variant is not the same as Direction A or Direction B. Direction names describe wall axes; variants describe selectable assets on those axes.

Each wall variant still follows the same common shape: one Direction A wall part, one Direction B wall part, and one Pillar part. Wall step is usually shared by the whole wall set.

The same VID may appear in multiple wall variants when its sprite `direction` value changes.

## Wall Direction Value

The numeric `direction` field stored on a `.map` sprite record.

Wall Direction Value is not Direction A or Direction B. Direction A/B names describe wall axes; Wall Direction Value controls sprite orientation or animation state inside the game map data.

## Locked Wall Variant Group

A group of wall variants that must be selected together because their direction parts and pillars visually match each other.

Wall sets with directional pillar slices do not necessarily need a locked wall variant group. Their wall sprites may randomize independently when the visual series is compatible, while pillar slice directions remain tied to wall topology.

## Random Wall Variant Pool

A group of compatible wall variants that may be randomly selected within the pool.

Direction A, Direction B, and Pillar may all participate in random selection. If a part has only one compatible asset in the selected pool, that part is effectively fixed for that pool.

Some wall sets expose only one random profile when their compatible wall sprites can be freely selected from one visual series.

## Rare Wall Variant Cadence

An optional spacing rule for a wall set that inserts a rare wall variant after a random number of ordinary wall parts.

The cadence is direction-aware: Direction A and Direction B can each receive the rare variant without relying on the other direction's wall count.

Rare wall variants are used for visually strong wall sprites that should appear, but not dominate the wall set.

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

## Door Assembly

A group of sprites that represents one logical door opening in a wall set.

## Multi-Part Door Frame

A door frame can be assembled from multiple frame sprites around one logical door panel.

Do not assume a door assembly always has one frame sprite at the same anchor as the panel. Some wall sets place frame parts beside the panel, and each frame part has its own direction and offset from the panel anchor.

## Door Panel State

The open or closed visual state of a door panel.

## Compensation Pillar

A pillar sprite added beside a door assembly when the door art does not visually include the full pillar edge.

Compensation pillar direction matches the pillar direction of the active wall variant group.

## Directional Pillar Slice

A pillar sprite that represents only one visible direction-specific slice of a pillar.

Directional pillar slices are used by wall sets where an endpoint, corner, or intersection is assembled from multiple direction-specific pillar pieces instead of one complete pillar sprite.

Directional pillar slices follow the same logical pillar trigger points as a normal pillar. The difference is that one logical pillar point may expand into one or more slice sprites, one for each connected wall direction.

## Multi-Part Pillar

A pillar generation mode where one logical pillar point can produce multiple pillar sprites.

Multi-part pillars are shared behavior for wall sets whose pillar visuals are assembled from direction-specific pieces. They are not specific to one wall set.

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

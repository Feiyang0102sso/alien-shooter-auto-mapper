# Project Glossary

## Map Format

Alien Shooter map file variant supported by the writer and binary templates.

## Project Version

The authoritative Map Format selected for an editor project and its generated `.map` output.

## AS1

Alien Shooter 1 / Zombie Shooter 1 map format.

## AS2

Alien Shooter 2 legacy map format. Its empty map template uses map version `0x13`.

## AS2R

Unified name for Alien Shooter 2 Reloaded, Alien Shooter 2 Conscription, and Zombie Shooter 2 compatible map format. Its empty map template uses map version `0x14`.

## AS2 Series

The family of AS2 and AS2R map formats; AS2R is not an alias for the legacy AS2 map format.

## Global Door State Override

An AS1-only project option that forces supported active doors open or closed; AS2 Series map formats ignore it.

## Project Format Relationships

- A **Project Version** selects exactly one **Map Format**.
- An AS2 Series **Project Version** ignores the **Global Door State Override**.
- **Direction Randomization** affects eligible walls and floors in every **Map Format**, plus AS1 pillars.
- AS2 pillars, **Door Flank Parts**, and topology-locked wall parts are **Fixed-Direction Parts**.
- Every **AS2 Wall Set Asset** has an explicit **Sprite Army Value**, including assets whose value is zero.
- An **AS2 Wall Set Asset** keeps the same **Sprite Army Value** everywhere its VID is used.
- An **AS2 Wall Set Preview** is shared by all **AS2 Series** Project Versions.
- **Sprite Army Value** is independent of **Map Format**; AS1 support may use the same VID binding in a separate implementation step.

## Wall Set

A group of wall sprites that share the same physical grid step and visual style.

## AS2 Wall Set Asset

Any sprite asset belonging to an AS2 wall set's walls, door assembly, Door Flank Parts, or matching floor.

## AS2 Wall Set Preview

An editor image that visually represents an AS2 Wall Set Asset or a complete AS2 Wall Set across AS2 Series Project Versions.

## Sprite Army Value

The game-defined numeric allegiance bound to an asset's VID across Map Formats and stored on its `.map` sprite record.

## Wall Set Legend

A non-interactive summary of every drawable part available in one wall set.

## Wall Variant

A visual asset choice inside a wall set. A wall set may have different variant counts for direction parts and pillar parts.

Wall variant is not the same as Direction A or Direction B. Direction names describe wall axes; variants describe selectable assets on those axes.

Each wall variant still follows the same common shape: one Direction A wall part, one Direction B wall part, and one Pillar part. Wall step is usually shared by the whole wall set.

The same VID may appear in multiple wall variants when its sprite `direction` value changes.

## Wall Direction Value

The numeric `direction` field stored on a `.map` sprite record.

Wall Direction Value is not Direction A or Direction B. Direction A/B names describe wall axes; Wall Direction Value controls sprite orientation or animation state inside the game map data.

## Direction Randomization

An output option that varies eligible Wall Direction Values when enabled and uses each eligible part's default direction when disabled.

Each eligible sprite chooses its direction independently; a wall segment or room does not share one random direction.

A fixed Wall Variant remains fixed regardless of this option; a random Wall Variant uses its first allowed direction as the default when the option is disabled.

Rare wall variants participate in Direction Randomization: they keep their configured cadence when randomization is enabled and do not appear when it is disabled.

AS2 pillars, Door Flank Parts, and topology-locked wall parts are Fixed-Direction Parts and never participate in Direction Randomization.

## Fixed-Direction Part

A sprite part whose Wall Direction Value is prescribed by its wall or door topology and is unaffected by Direction Randomization.

## Locked Wall Variant Group

A group of wall variants that must be selected together because their direction parts and pillars visually match each other.

Wall sets with directional pillar slices do not necessarily need a locked wall variant group. Their wall sprites may randomize independently when the visual series is compatible, while pillar slice directions remain tied to wall topology.

## Random Wall Variant Pool

A group of compatible wall variants that may be randomly selected within the pool.

Direction A, Direction B, and Pillar may all participate in random selection. If a part has only one compatible asset in the selected pool, that part is effectively fixed for that pool.

Some wall sets expose only one random profile when their compatible wall sprites can be freely selected from one visual series.

## Rare Wall Variant Density

An optional scarcity rule that limits how many eligible wall parts may receive a rare wall variant.

Rare wall variant density is direction-aware: Direction A and Direction B receive separate placement budgets.

The density is a target proportion rather than a guaranteed result; spatial placement constraints may reduce the number of rare wall variants.

Each Direction budget rounds down the post-excavation wall-part count multiplied by the target density, before feature-placement constraints are applied.

Rare wall variants are used for visually strong wall sprites that should appear, but not dominate the wall set.

_Avoid_: Rare Wall Variant Cadence, because rare variants are not inserted at traversal intervals.

## Straight Wall Run

A maximal uninterrupted sequence of wall parts in one wall direction, bounded by an endpoint, corner, junction, or door opening.

## Rare Wall Feature Placement

A spatial rule that treats a rare wall variant as a visual focal part placed within a continuous straight wall run and kept away from wall endpoints, corners, and doors.

An eligible rare wall position has at least two neighboring wall parts on both sides within the same Straight Wall Run.

Rare wall feature placement constrains candidates from both Direction A and Direction B within the same Wall Set so independently budgeted rare variants do not visually cluster.

Rare wall feature placement does not impose spatial separation between different Wall Sets.

Rare wall feature placement measures visual separation in Map Coordinates rather than Grid Coordinates.

Eligible Straight Wall Runs receive rare wall placement opportunities in rounds before any one run receives an additional opportunity.

Each randomized generation may select different eligible rare wall positions while preserving the same density and spatial constraints.

Eligible positions within a Straight Wall Run are selected uniformly without preference for the run center.

If no candidate satisfies the spatial rule, the wall run contains no rare wall variant; placement constraints are not relaxed to force one.

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

## Door Flank Part

An arbitrary wall-class sprite (pillar, air-wall, or regular wall) placed next to a door assembly to visually seal the frame gap.

Door Flank Parts are declared per door direction in the profile (Direction A and Direction B independently), each carrying its own asset id, sprite direction, and map-coordinate offset from the frame anchor. A direction can carry zero, one, or many flank parts — symmetric (one on each side) or asymmetric (only one side).

Flank parts are emitted in the DoorBuilder pass, NOT in WallBuilder, so they survive the WallBuilder door-opening excavation pass. Do not emit flank parts in WallBuilder.

_Avoid:_ Compensation Pillar (pillar-specific legacy term).

## Compensation Pillar

Legacy name for a Door Flank Part when the asset happens to be a pillar sprite. Retained for historical references but new glossary entries use **Door Flank Part**.

## Directional Pillar Slice

A pillar sprite that represents only one visible direction-specific slice of a pillar.

Directional pillar slices are used by wall sets where an endpoint, corner, or intersection is assembled from multiple direction-specific pillar pieces instead of one complete pillar sprite.

Directional pillar slices follow the same logical pillar trigger points as a normal pillar. The difference is that one logical pillar point may expand into one or more slice sprites, one for each connected wall direction.

## Multi-Part Pillar

A pillar generation mode where one logical pillar point can produce multiple pillar sprites.

Multi-part pillars are shared behavior for wall sets whose pillar visuals are assembled from direction-specific pieces. They are not specific to one wall set.

## Ceiling Curtains

### Exterior Wall Part

A wall part that separates room interior from map exterior.

### Concave Wall Recess (凹角)

An inward recess in the room's exterior wall boundary formed by two opposing side walls and one connecting wall.

### Deep Recess Corner（凹槽深角）

A concave boundary vertex whose four neighboring grid cells contain exactly one exterior cell.

Deep Recess Corners are classified by the exterior cell's position relative to the vertex:

```text
Upper-left     Upper-right    Lower-left     Lower-right
■ □            □ ■            □ □            □ □
□ □            □ □            ■ □            □ ■
```

Screen-facing descriptions such as “upward corner” or “downward corner” are avoided because they do not uniquely identify Grid topology.

### Deep Corner Replacement Curtain（深角替代帘）

A Long Ceiling Curtain that replaces a removed overhanging corner curtain using the same asset and direction, with only its along-wall position changed.

### Ceiling Curtain

An AS2 Series black rectangular cover outside Exterior Wall Parts that conceals the map exterior.

_Avoid:_ Ceiling, because the legacy AS1 ceiling tile has different geometry and placement semantics.

### Long Ceiling Curtain (长)

The default long rectangle used for ordinary Exterior Wall Parts, with one Long Ceiling Curtain for each wall part.

In project dialogue, call this curtain **长**.

### Wide Ceiling Curtain (宽)

A wide rectangle used at a Concave Wall Recess entrance when a facing parallel wall is within three wall-part widths.

The width of one Wide Ceiling Curtain is approximately three wall-part widths.

In project dialogue, call this curtain **宽**.

### Ceiling Curtain Direction Values

All AS2 Wall Sets share these Ceiling Curtain direction values:

- Direction A (`/`) **长**: `96`
- Direction B (`\`) **长**: `32`
- Direction A (`/`) **宽**: `158`
- Direction B (`\`) **宽**: `97`

Ceiling Curtain relationships:

- Each ordinary **Exterior Wall Part** receives one **长**.
- A **Deep Recess Corner** has exactly one of four quadrant variants: Upper-left, Upper-right, Lower-left, or Lower-right.
- A **Deep Corner Replacement Curtain** preserves the removed curtain's asset, direction, and outward alignment while changing only its along-wall position.
- Each side of a **Concave Wall Recess** is evaluated independently.
- A recess entrance uses **宽** when a facing parallel wall of the same direction is one to three wall-part widths away.
- A facing parallel wall exactly three wall-part widths away still uses **宽**.
- A recess entrance uses **长** when the facing parallel wall is more than three wall-part widths away or no facing parallel wall exists.
- The two opposing sides of a narrow **Concave Wall Recess** each receive their own **宽**.
- The connecting wall of a **Concave Wall Recess** continues to use **长** when no facing parallel wall of its direction exists within three wall-part widths.
- One **宽** is approximately as wide as three **Exterior Wall Parts**.
- At a concave turn, the adjacent `\` **长** covers the corner. A terminal `/` **长** that is not replaced by **宽** is omitted so its visual tail cannot cross beyond the turn.
- Facing narrow wall parts are divided into consecutive groups of up to three, starting at the Concave Wall Recess entrance.
- Each group receives one **宽**, and none of its covered wall parts receive **长**.
- When the final group has fewer than three wall parts, its **宽** shifts back toward the entrance so it does not extend past the recess end.
- Wall parts after the facing narrow relationship ends continue to receive their normal **长**.

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

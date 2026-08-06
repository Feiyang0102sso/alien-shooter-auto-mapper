"""
Infer AS2 door profile candidates from an unpacked door calibration map.

The script reads JSON produced by as2_map_unpack.py. It compares the door
sample against a matching wall-set sample, then groups non-wall sprites that
share the same anchor into door assemblies:

    frame sprite + open/closed panel sprite + optional compensation pillar

Open/closed names can be passed as hints. Without hints, two-state panels are
reported with the smaller VID treated as closed and the larger VID as open.
"""

import argparse
import json
from dataclasses import dataclass
from pathlib import Path

from infer_wall_steps import build_report
from infer_wall_steps import load_sprites
from infer_wall_steps import rounded_number


REPORT_FILE_NAME = "_door_profile_report.json"
FLOAT_TOLERANCE = 0.001
MIN_FRAME_COOCCURRENCE_COUNT = 2
MAX_PILLAR_OFFSET_STEPS = 3.0


@dataclass(frozen=True)
class DoorSprite:
    """Minimal sprite data needed for door profile inference."""

    vid: int
    pos_x: float
    pos_y: float
    direction: int
    army: int


@dataclass(frozen=True)
class FrameAxisSample:
    """A repeated frame direction delta mapped to one wall axis."""

    frame_direction: int
    axis_name: str
    grid_distance: int
    dx: float
    dy: float


@dataclass(frozen=True)
class PillarOffsetCandidate:
    """A repeated pillar offset near a door frame."""

    offset_x: float
    offset_y: float
    count: int


@dataclass(frozen=True)
class PanelAxisSample:
    """A repeated panel direction delta mapped to one wall axis."""

    panel_direction: int
    axis_name: str
    grid_distance: int
    dx: float
    dy: float


@dataclass(frozen=True)
class FramePartCandidate:
    """A frame part placed at a stable offset from a panel anchor."""

    frame_vid: int
    frame_direction: int
    axis_name: str
    offset_x: float
    offset_y: float
    count: int


@dataclass(frozen=True)
class TargetWallVariant:
    """Wall variant directions used by the target door profile."""

    dir_a_direction: int
    dir_b_direction: int
    pillar_direction: int


def parse_vid_list(raw_value: str | None) -> set[int]:
    """Parse a comma-separated VID list."""
    vids: set[int] = set()

    if raw_value is None:
        return vids

    parts = raw_value.split(",")
    for part in parts:
        text = part.strip()
        if text == "":
            continue
        vids.add(int(text))

    return vids


def parse_optional_int(raw_value: str | None) -> int | None:
    """Parse an optional integer argument."""
    if raw_value is None:
        return None

    text = raw_value.strip()
    if text == "":
        return None

    return int(text)


def load_door_sprites(path: Path) -> list[DoorSprite]:
    """Load sprite points from an unpacked AS2 map JSON file."""
    data = json.loads(path.read_text(encoding="utf-8"))
    sprite_section = data.get("SPR", {})
    raw_sprites = sprite_section.get("sprites", [])

    sprites: list[DoorSprite] = []
    for raw_sprite in raw_sprites:
        sprite = DoorSprite(
            vid=int(raw_sprite["vid"]),
            pos_x=float(raw_sprite["posX"]),
            pos_y=float(raw_sprite["posY"]),
            direction=int(raw_sprite.get("direction", 0)),
            army=int(raw_sprite.get("army", 0)),
        )
        sprites.append(sprite)

    return sprites


def get_anchor_key(sprite: DoorSprite) -> tuple[float, float]:
    """Return a stable coordinate key for anchor grouping."""
    return rounded_number(sprite.pos_x), rounded_number(sprite.pos_y)


def group_by_anchor(sprites: list[DoorSprite]) -> dict[tuple[float, float], list[DoorSprite]]:
    """Group sprites by exact rounded map anchor."""
    groups: dict[tuple[float, float], list[DoorSprite]] = {}

    for sprite in sprites:
        key = get_anchor_key(sprite)
        if key not in groups:
            groups[key] = []
        groups[key].append(sprite)

    return groups


def collect_vids(sprites: list[DoorSprite]) -> set[int]:
    """Collect all VIDs in a sprite list."""
    vids: set[int] = set()

    for sprite in sprites:
        vids.add(sprite.vid)

    return vids


def build_wall_report(wall_json: Path) -> dict:
    """Run wall inference for the matching wall-set sample."""
    empty_vids: set[int] = set()
    return build_report(wall_json, empty_vids, empty_vids, empty_vids)


def collect_wall_vids(wall_report: dict) -> set[int]:
    """Collect direction and pillar VIDs from a wall inference report."""
    vids: set[int] = set()

    for item in wall_report.get("dir_a_like_negative_x_vids", []):
        vids.add(int(item["vid"]))

    for item in wall_report.get("dir_b_like_positive_x_vids", []):
        vids.add(int(item["vid"]))

    for vid in wall_report.get("pillar_or_corner_vid_candidates", []):
        vids.add(int(vid))

    return vids


def find_candidate_door_sprites(sprites: list[DoorSprite], wall_vids: set[int]) -> list[DoorSprite]:
    """Return sprites that are not part of the matching wall set."""
    selected: list[DoorSprite] = []

    for sprite in sprites:
        if sprite.vid in wall_vids:
            continue
        selected.append(sprite)

    return selected


def infer_frame_vids(
    candidate_sprites: list[DoorSprite],
    frame_hints: set[int],
) -> list[int]:
    """Infer door frame VIDs from repeated same-anchor co-occurrence."""
    if len(frame_hints) > 0:
        return sorted(frame_hints)

    groups = group_by_anchor(candidate_sprites)
    cooccurrence: dict[int, set[int]] = {}
    anchor_counts: dict[int, int] = {}

    for group in groups.values():
        if len(group) < 2:
            continue

        group_vids = collect_vids(group)
        for sprite in group:
            if sprite.vid not in cooccurrence:
                cooccurrence[sprite.vid] = set()
                anchor_counts[sprite.vid] = 0

            anchor_counts[sprite.vid] += 1
            for other_vid in group_vids:
                if other_vid == sprite.vid:
                    continue
                cooccurrence[sprite.vid].add(other_vid)

    frame_vids: list[int] = []
    for vid, other_vids in cooccurrence.items():
        if len(other_vids) < MIN_FRAME_COOCCURRENCE_COUNT:
            continue
        frame_vids.append(vid)

    frame_vids.sort()
    return frame_vids


def find_sprites_by_vid(sprites: list[DoorSprite], vid: int) -> list[DoorSprite]:
    """Return sprites with one VID."""
    selected: list[DoorSprite] = []

    for sprite in sprites:
        if sprite.vid == vid:
            selected.append(sprite)

    return selected


def normalize_delta(first: DoorSprite, second: DoorSprite) -> tuple[float, float] | None:
    """Return a delta with positive Y so both pair orders collapse together."""
    dx = rounded_number(second.pos_x - first.pos_x)
    dy = rounded_number(second.pos_y - first.pos_y)

    if abs(dx) <= FLOAT_TOLERANCE:
        return None

    if abs(dy) <= FLOAT_TOLERANCE:
        return None

    if dy < 0:
        dx = -dx
        dy = -dy

    return dx, dy


def delta_to_grid_distance(dx: float, dy: float, step_x: float, step_y: float) -> int | None:
    """Convert an isometric delta to a positive grid distance."""
    x_steps = abs(dx) / step_x
    y_steps = abs(dy) / step_y

    rounded_x_steps = round(x_steps)
    rounded_y_steps = round(y_steps)

    if abs(x_steps - rounded_x_steps) > FLOAT_TOLERANCE:
        return None

    if abs(y_steps - rounded_y_steps) > FLOAT_TOLERANCE:
        return None

    if rounded_x_steps != rounded_y_steps:
        return None

    if rounded_x_steps <= 0:
        return None

    return int(rounded_x_steps)


def infer_frame_axis_samples(
    frame_sprites: list[DoorSprite],
    step_x: float,
    step_y: float,
) -> list[FrameAxisSample]:
    """Infer axis names for each frame direction value."""
    direction_values: set[int] = set()
    for sprite in frame_sprites:
        direction_values.add(sprite.direction)

    samples: list[FrameAxisSample] = []
    for direction in sorted(direction_values):
        same_direction_sprites: list[DoorSprite] = []
        for sprite in frame_sprites:
            if sprite.direction == direction:
                same_direction_sprites.append(sprite)

        best_sample: FrameAxisSample | None = None
        for first_index in range(0, len(same_direction_sprites)):
            first = same_direction_sprites[first_index]
            for second_index in range(first_index + 1, len(same_direction_sprites)):
                second = same_direction_sprites[second_index]
                delta = normalize_delta(first, second)
                if delta is None:
                    continue

                dx, dy = delta
                grid_distance = delta_to_grid_distance(dx, dy, step_x, step_y)
                if grid_distance is None:
                    continue

                axis_name = "dir_a"
                if dx > 0:
                    axis_name = "dir_b"

                sample = FrameAxisSample(
                    frame_direction=direction,
                    axis_name=axis_name,
                    grid_distance=grid_distance,
                    dx=dx,
                    dy=dy,
                )

                if best_sample is None:
                    best_sample = sample
                    continue

                if sample.grid_distance < best_sample.grid_distance:
                    best_sample = sample

        if best_sample is not None:
            samples.append(best_sample)

    return samples


def infer_span_steps(axis_samples: list[FrameAxisSample]) -> int:
    """Infer door span from repeated frame anchor spacing."""
    min_grid_distance: int | None = None

    for sample in axis_samples:
        if min_grid_distance is None:
            min_grid_distance = sample.grid_distance
            continue

        if sample.grid_distance < min_grid_distance:
            min_grid_distance = sample.grid_distance

    if min_grid_distance is None:
        return 1

    span_steps = min_grid_distance - 1
    if span_steps < 1:
        return 1

    return span_steps


def map_frame_directions(axis_samples: list[FrameAxisSample]) -> dict:
    """Format frame direction mapping for JSON output."""
    result: dict = {}

    for sample in axis_samples:
        result[sample.axis_name] = {
            "frame_direction": sample.frame_direction,
            "repeat_delta": [sample.dx, sample.dy],
            "grid_distance": sample.grid_distance,
        }

    return result


def collect_state_vids_for_frame(
    frame_vid: int,
    candidate_sprites: list[DoorSprite],
    frame_vids: set[int],
) -> set[int]:
    """Collect non-frame VIDs paired with one frame VID at the same anchor."""
    groups = group_by_anchor(candidate_sprites)
    state_vids: set[int] = set()

    for group in groups.values():
        has_frame = False
        for sprite in group:
            if sprite.vid == frame_vid:
                has_frame = True
                break

        if not has_frame:
            continue

        for sprite in group:
            if sprite.vid in frame_vids:
                continue
            state_vids.add(sprite.vid)

    return state_vids


def classify_state_vids(
    state_vids: set[int],
    open_hints: set[int],
    closed_hints: set[int],
) -> dict:
    """Classify panel VIDs as open/closed when possible."""
    result: dict = {
        "states": [],
        "assumption": None,
    }

    for vid in sorted(state_vids):
        role = "unknown"
        if vid in open_hints:
            role = "open"
        if vid in closed_hints:
            role = "closed"

        result["states"].append({"vid": vid, "role": role})

    has_hints = len(open_hints) > 0 or len(closed_hints) > 0
    if has_hints:
        return result

    if len(state_vids) == 2:
        sorted_vids = sorted(state_vids)
        result["states"] = [
            {"vid": sorted_vids[0], "role": "closed"},
            {"vid": sorted_vids[1], "role": "open"},
        ]
        result["assumption"] = "No state hints were provided; smaller VID is treated as closed."

    return result


def infer_panel_direction_map(
    frame_vid: int,
    state_vids: set[int],
    candidate_sprites: list[DoorSprite],
    axis_samples: list[FrameAxisSample],
) -> dict:
    """Infer panel direction values by same-anchor pairing with the frame."""
    frame_direction_to_axis: dict[int, str] = {}
    for sample in axis_samples:
        frame_direction_to_axis[sample.frame_direction] = sample.axis_name

    groups = group_by_anchor(candidate_sprites)
    result: dict = {}

    for group in groups.values():
        frame_sprite: DoorSprite | None = None
        for sprite in group:
            if sprite.vid == frame_vid:
                frame_sprite = sprite
                break

        if frame_sprite is None:
            continue

        axis_name = frame_direction_to_axis.get(frame_sprite.direction)
        if axis_name is None:
            continue

        for sprite in group:
            if sprite.vid not in state_vids:
                continue

            vid_key = str(sprite.vid)
            if vid_key not in result:
                result[vid_key] = {}
            result[vid_key][axis_name] = sprite.direction

    return result


def find_panel_sprites(candidate_sprites: list[DoorSprite], state_vids: set[int]) -> list[DoorSprite]:
    """Return panel sprites matching inferred or hinted state VIDs."""
    panel_sprites: list[DoorSprite] = []

    for sprite in candidate_sprites:
        if sprite.vid in state_vids:
            panel_sprites.append(sprite)

    return panel_sprites


def infer_panel_axis_samples(
    panel_sprites: list[DoorSprite],
    step_x: float,
    step_y: float,
) -> list[PanelAxisSample]:
    """Infer axis names for each panel direction value."""
    direction_values: set[int] = set()
    for sprite in panel_sprites:
        direction_values.add(sprite.direction)

    samples: list[PanelAxisSample] = []
    for direction in sorted(direction_values):
        same_direction_sprites: list[DoorSprite] = []
        for sprite in panel_sprites:
            if sprite.direction == direction:
                same_direction_sprites.append(sprite)

        best_sample: PanelAxisSample | None = None
        for first_index in range(0, len(same_direction_sprites)):
            first = same_direction_sprites[first_index]
            for second_index in range(first_index + 1, len(same_direction_sprites)):
                second = same_direction_sprites[second_index]
                delta = normalize_delta(first, second)
                if delta is None:
                    continue

                dx, dy = delta
                grid_distance = delta_to_grid_distance(dx, dy, step_x, step_y)
                if grid_distance is None:
                    continue

                axis_name = "dir_a"
                if dx > 0:
                    axis_name = "dir_b"

                sample = PanelAxisSample(
                    panel_direction=direction,
                    axis_name=axis_name,
                    grid_distance=grid_distance,
                    dx=dx,
                    dy=dy,
                )

                if best_sample is None:
                    best_sample = sample
                    continue

                if sample.grid_distance < best_sample.grid_distance:
                    best_sample = sample

        if best_sample is not None:
            samples.append(best_sample)

    return samples


def map_panel_directions_from_panel_samples(
    state_vids: set[int],
    panel_sprites: list[DoorSprite],
    panel_axis_samples: list[PanelAxisSample],
) -> dict:
    """Format panel direction mapping when frames are offset from panels."""
    direction_to_axis: dict[int, str] = {}
    for sample in panel_axis_samples:
        direction_to_axis[sample.panel_direction] = sample.axis_name

    result: dict = {}
    for sprite in panel_sprites:
        if sprite.vid not in state_vids:
            continue

        axis_name = direction_to_axis.get(sprite.direction)
        if axis_name is None:
            continue

        vid_key = str(sprite.vid)
        if vid_key not in result:
            result[vid_key] = {}
        result[vid_key][axis_name] = sprite.direction

    return result


def infer_panel_centered_frame_parts(
    frame_vid: int,
    panel_sprites: list[DoorSprite],
    frame_sprites: list[DoorSprite],
    panel_axis_samples: list[PanelAxisSample],
    step_x: float,
    step_y: float,
) -> list[FramePartCandidate]:
    """Infer frame parts placed near panel anchors."""
    direction_to_axis: dict[int, str] = {}
    for sample in panel_axis_samples:
        direction_to_axis[sample.panel_direction] = sample.axis_name

    max_offset_x = step_x
    max_offset_y = step_y
    part_counts: dict[tuple[int, str, float, float], int] = {}

    for panel in panel_sprites:
        axis_name = direction_to_axis.get(panel.direction)
        if axis_name is None:
            continue

        for frame in frame_sprites:
            offset_x = rounded_number(frame.pos_x - panel.pos_x)
            offset_y = rounded_number(frame.pos_y - panel.pos_y)

            if abs(offset_x) > max_offset_x:
                continue

            if abs(offset_y) > max_offset_y:
                continue

            key = (frame.direction, axis_name, offset_x, offset_y)
            if key not in part_counts:
                part_counts[key] = 0
            part_counts[key] += 1

    candidates: list[FramePartCandidate] = []
    for key, count in part_counts.items():
        frame_direction, axis_name, offset_x, offset_y = key
        candidate = FramePartCandidate(
            frame_vid=frame_vid,
            frame_direction=frame_direction,
            axis_name=axis_name,
            offset_x=offset_x,
            offset_y=offset_y,
            count=count,
        )
        candidates.append(candidate)

    candidates.sort(key=frame_part_sort_key)
    return candidates


def frame_part_sort_key(candidate: FramePartCandidate) -> tuple[str, float, float, int]:
    """Sort frame parts by axis and visual left/right offset."""
    return candidate.axis_name, candidate.offset_x, candidate.offset_y, candidate.frame_direction


def format_frame_parts(candidates: list[FramePartCandidate]) -> list[dict]:
    """Format frame part candidates for JSON output."""
    items: list[dict] = []

    for candidate in candidates:
        item = {
            "frame_vid": candidate.frame_vid,
            "frame_direction": candidate.frame_direction,
            "axis": candidate.axis_name,
            "offset_from_panel": [candidate.offset_x, candidate.offset_y],
            "repeat_count": candidate.count,
        }
        items.append(item)

    return items


def find_nearby_pillar_offsets(
    frame_sprites: list[DoorSprite],
    pillar_sprites: list[DoorSprite],
    step_x: float,
    step_y: float,
) -> list[PillarOffsetCandidate]:
    """Find repeated pillar offsets near frame anchors."""
    max_offset_x = step_x * MAX_PILLAR_OFFSET_STEPS
    max_offset_y = step_y * MAX_PILLAR_OFFSET_STEPS
    offset_counts: dict[tuple[float, float], int] = {}

    for frame in frame_sprites:
        for pillar in pillar_sprites:
            offset_x = rounded_number(pillar.pos_x - frame.pos_x)
            offset_y = rounded_number(pillar.pos_y - frame.pos_y)

            if abs(offset_x) > max_offset_x:
                continue

            if abs(offset_y) > max_offset_y:
                continue

            if abs(offset_x) <= FLOAT_TOLERANCE and abs(offset_y) <= FLOAT_TOLERANCE:
                continue

            offset = (offset_x, offset_y)
            if offset not in offset_counts:
                offset_counts[offset] = 0
            offset_counts[offset] += 1

    candidates: list[PillarOffsetCandidate] = []
    for offset, count in offset_counts.items():
        if count < 2:
            continue

        offset_x, offset_y = offset
        candidate = PillarOffsetCandidate(offset_x=offset_x, offset_y=offset_y, count=count)
        candidates.append(candidate)

    candidates.sort(key=pillar_offset_sort_key)
    return candidates


def pillar_offset_sort_key(candidate: PillarOffsetCandidate) -> tuple[int, float]:
    """Sort strongest and nearest pillar offsets first."""
    distance = abs(candidate.offset_x) + abs(candidate.offset_y)
    return -candidate.count, distance


def format_pillar_offsets(candidates: list[PillarOffsetCandidate]) -> list[dict]:
    """Format pillar offset candidates for JSON output."""
    items: list[dict] = []

    for candidate in candidates:
        item = {
            "offset_x": candidate.offset_x,
            "offset_y": candidate.offset_y,
            "repeat_count": candidate.count,
        }
        items.append(item)

    return items


def build_frame_report(
    frame_vid: int,
    candidate_sprites: list[DoorSprite],
    frame_vids: set[int],
    wall_sprites: list[DoorSprite],
    wall_report: dict,
    open_hints: set[int],
    closed_hints: set[int],
    target_wall_variant: TargetWallVariant | None,
) -> dict:
    """Build one door assembly report."""
    step_x = float(wall_report["step_x"])
    step_y = float(wall_report["step_y"])
    pillar_vids = set()
    for vid in wall_report.get("pillar_or_corner_vid_candidates", []):
        pillar_vids.add(int(vid))

    frame_sprites = find_sprites_by_vid(candidate_sprites, frame_vid)
    axis_samples = infer_frame_axis_samples(frame_sprites, step_x, step_y)
    state_vids = collect_state_vids_for_frame(frame_vid, candidate_sprites, frame_vids)
    if len(state_vids) == 0:
        hinted_state_vids = open_hints | closed_hints
        candidate_vids = collect_vids(candidate_sprites)
        for vid in hinted_state_vids:
            if vid in candidate_vids:
                state_vids.add(vid)

    state_report = classify_state_vids(state_vids, open_hints, closed_hints)
    panel_direction_map = infer_panel_direction_map(
        frame_vid=frame_vid,
        state_vids=state_vids,
        candidate_sprites=candidate_sprites,
        axis_samples=axis_samples,
    )
    panel_sprites = find_panel_sprites(candidate_sprites, state_vids)
    panel_axis_samples = infer_panel_axis_samples(panel_sprites, step_x, step_y)
    if len(panel_direction_map) == 0:
        panel_direction_map = map_panel_directions_from_panel_samples(
            state_vids=state_vids,
            panel_sprites=panel_sprites,
            panel_axis_samples=panel_axis_samples,
        )

    frame_parts = infer_panel_centered_frame_parts(
        frame_vid=frame_vid,
        panel_sprites=panel_sprites,
        frame_sprites=frame_sprites,
        panel_axis_samples=panel_axis_samples,
        step_x=step_x,
        step_y=step_y,
    )

    pillar_sprites: list[DoorSprite] = []
    for sprite in wall_sprites:
        if sprite.vid in pillar_vids:
            pillar_sprites.append(sprite)

    span_steps = infer_span_steps(axis_samples)
    pillar_offsets: list[PillarOffsetCandidate] = []
    if span_steps >= 2:
        pillar_offsets = find_nearby_pillar_offsets(frame_sprites, pillar_sprites, step_x, step_y)

    needs_compensation_pillar = len(pillar_offsets) > 0
    compensation_reason = "nearby_pillar_offsets_found"
    if len(pillar_offsets) == 0:
        compensation_reason = "no_repeated_nearby_pillar_offsets"

    if target_wall_variant is not None:
        # The wall builder already emits the pillar direction for this wall variant.
        # A door profile should not duplicate it as a compensation pillar.
        needs_compensation_pillar = False
        compensation_reason = "covered_by_wall_variant_pillar"

    target_wall_variant_report = None
    if target_wall_variant is not None:
        target_wall_variant_report = {
            "dir_a_direction": target_wall_variant.dir_a_direction,
            "dir_b_direction": target_wall_variant.dir_b_direction,
            "pillar_direction": target_wall_variant.pillar_direction,
        }

    return {
        "frame_vid": frame_vid,
        "span_steps": span_steps,
        "target_wall_variant": target_wall_variant_report,
        "frame_direction_map": map_frame_directions(axis_samples),
        "frame_parts": format_frame_parts(frame_parts),
        "panel_vids": state_report["states"],
        "state_assumption": state_report["assumption"],
        "panel_direction_map": panel_direction_map,
        "needs_compensation_pillar": needs_compensation_pillar,
        "compensation_pillar_reason": compensation_reason,
        "compensation_pillar_vid_candidates": sorted(pillar_vids),
        "nearby_pillar_offset_candidates": format_pillar_offsets(pillar_offsets),
    }


def build_report_for_doors(
    door_json: Path,
    wall_json: Path,
    frame_hints: set[int],
    open_hints: set[int],
    closed_hints: set[int],
    target_wall_variant: TargetWallVariant | None,
) -> dict:
    """Build the complete door inference report."""
    wall_report = build_wall_report(wall_json)
    wall_vids = collect_wall_vids(wall_report)

    all_sprites = load_door_sprites(door_json)
    candidate_sprites = find_candidate_door_sprites(all_sprites, wall_vids)
    frame_vids = infer_frame_vids(candidate_sprites, frame_hints)
    frame_vid_set = set(frame_vids)

    door_assemblies: list[dict] = []
    for frame_vid in frame_vids:
        frame_report = build_frame_report(
            frame_vid=frame_vid,
            candidate_sprites=candidate_sprites,
            frame_vids=frame_vid_set,
            wall_sprites=all_sprites,
            wall_report=wall_report,
            open_hints=open_hints,
            closed_hints=closed_hints,
            target_wall_variant=target_wall_variant,
        )
        door_assemblies.append(frame_report)

    return {
        "door_source": str(door_json),
        "wall_source": str(wall_json),
        "status": "ok",
        "wall_step": {
            "step_x": wall_report["step_x"],
            "step_y": wall_report["step_y"],
        },
        "wall_vids": sorted(wall_vids),
        "candidate_door_vids": sorted(collect_vids(candidate_sprites)),
        "door_assemblies": door_assemblies,
    }


def print_report(report: dict) -> None:
    """Print a concise human-readable report."""
    print(f"Door source: {report['door_source']}")
    print(f"Wall source: {report['wall_source']}")
    print(f"Status: {report['status']}")
    print(f"Step: {report['wall_step']['step_x']} x {report['wall_step']['step_y']}")
    print(f"Wall VIDs: {report['wall_vids']}")
    print(f"Door VID candidates: {report['candidate_door_vids']}")
    print("Door assemblies:")

    for assembly in report["door_assemblies"]:
        print(
            f"  frame={assembly['frame_vid']} span={assembly['span_steps']} "
            f"needs_pillar={assembly['needs_compensation_pillar']}"
        )
        print(f"    frame dirs: {assembly['frame_direction_map']}")
        print(f"    frame parts: {assembly['frame_parts']}")
        print(f"    panels: {assembly['panel_vids']}")
        print(f"    panel dirs: {assembly['panel_direction_map']}")
        if assembly["state_assumption"] is not None:
            print(f"    state assumption: {assembly['state_assumption']}")
        if assembly["needs_compensation_pillar"]:
            print(f"    pillar vids: {assembly['compensation_pillar_vid_candidates']}")
            print(f"    pillar offsets: {assembly['nearby_pillar_offset_candidates']}")
        print(f"    pillar reason: {assembly['compensation_pillar_reason']}")


def infer_default_wall_json(door_json: Path) -> Path | None:
    """Infer the matching wall sample path from a wall_set folder."""
    folder = door_json.parent
    candidates: list[Path] = []

    for path in folder.glob("*.json"):
        if path.name == door_json.name:
            continue

        if path.name == REPORT_FILE_NAME:
            continue

        if path.name.startswith("_wall_step_report"):
            continue

        if path.name.startswith("_door_profile_report"):
            continue

        if path.name.endswith("_doors.json"):
            continue

        candidates.append(path)

    candidates.sort()
    if len(candidates) == 1:
        return candidates[0]

    return None


def parse_args() -> argparse.Namespace:
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Infer AS2 door VID, direction, span, and pillar profile candidates."
    )
    parser.add_argument(
        "door_json",
        type=Path,
        help="Unpacked door calibration JSON file.",
    )
    parser.add_argument(
        "--wall-json",
        type=Path,
        default=None,
        help="Matching unpacked wall-set JSON file. Defaults to the only non-door JSON beside the input.",
    )
    parser.add_argument(
        "--frame-vid",
        default=None,
        help="Optional comma-separated frame VID hints.",
    )
    parser.add_argument(
        "--open-vid",
        default=None,
        help="Optional comma-separated open panel VID hints.",
    )
    parser.add_argument(
        "--closed-vid",
        default=None,
        help="Optional comma-separated closed panel VID hints.",
    )
    parser.add_argument(
        "--target-dir-a-direction",
        default=None,
        help="Optional target wall variant Direction A sprite direction.",
    )
    parser.add_argument(
        "--target-dir-b-direction",
        default=None,
        help="Optional target wall variant Direction B sprite direction.",
    )
    parser.add_argument(
        "--target-pillar-direction",
        default=None,
        help="Optional target wall variant pillar direction.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=None,
        help="Optional output report JSON path. Defaults to _door_profile_report.json beside the input.",
    )
    return parser.parse_args()


def main() -> int:
    """CLI entry point."""
    args = parse_args()
    door_json = args.door_json

    if not door_json.exists():
        print(f"Error: door JSON does not exist: {door_json}")
        return 1

    wall_json = args.wall_json
    if wall_json is None:
        wall_json = infer_default_wall_json(door_json)

    if wall_json is None:
        print("Error: could not infer matching wall JSON. Pass --wall-json explicitly.")
        return 1

    if not wall_json.exists():
        print(f"Error: wall JSON does not exist: {wall_json}")
        return 1

    frame_hints = parse_vid_list(args.frame_vid)
    open_hints = parse_vid_list(args.open_vid)
    closed_hints = parse_vid_list(args.closed_vid)
    target_dir_a_direction = parse_optional_int(args.target_dir_a_direction)
    target_dir_b_direction = parse_optional_int(args.target_dir_b_direction)
    target_pillar_direction = parse_optional_int(args.target_pillar_direction)

    target_wall_variant = None
    has_target_wall_variant = False
    if target_dir_a_direction is not None:
        has_target_wall_variant = True
    if target_dir_b_direction is not None:
        has_target_wall_variant = True
    if target_pillar_direction is not None:
        has_target_wall_variant = True

    if has_target_wall_variant:
        if target_dir_a_direction is None:
            print("Error: --target-dir-a-direction is required when target wall variant hints are used.")
            return 1
        if target_dir_b_direction is None:
            print("Error: --target-dir-b-direction is required when target wall variant hints are used.")
            return 1
        if target_pillar_direction is None:
            print("Error: --target-pillar-direction is required when target wall variant hints are used.")
            return 1

        target_wall_variant = TargetWallVariant(
            dir_a_direction=target_dir_a_direction,
            dir_b_direction=target_dir_b_direction,
            pillar_direction=target_pillar_direction,
        )

    report = build_report_for_doors(
        door_json=door_json,
        wall_json=wall_json,
        frame_hints=frame_hints,
        open_hints=open_hints,
        closed_hints=closed_hints,
        target_wall_variant=target_wall_variant,
    )
    print_report(report)

    output_path = args.output
    if output_path is None:
        output_path = door_json.parent / REPORT_FILE_NAME

    output_text = json.dumps(report, indent=2, ensure_ascii=False)
    output_path.write_text(output_text, encoding="utf-8")
    print(f"Saved report: {output_path}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

"""
Infer wall step sizes from a small AS2 wall calibration room.

The script reads JSON produced by as2_map_unpack.py. It looks for repeated
same-VID sprite coordinates and derives the two isometric wall axes:

    +X axis candidate: (+step_x, +step_y)
    -X axis candidate: (-step_x, +step_y)

This is meant for 3x3 wall sample maps. A tiny room can infer shared wall
steps, but rare one-off variants may still need manual VID hints.
"""

import argparse
import json
from dataclasses import dataclass
from pathlib import Path


FLOAT_TOLERANCE = 0.001
MIN_AXIS_REPEAT_COUNT = 2
REPORT_FILE_NAME = "_wall_step_report.json"
MAX_OFFSET_GRID_STEPS = 12
MIN_OFFSET_REPEAT_COUNT = 2


@dataclass(frozen=True)
class SpritePoint:
    """Minimal sprite data needed for wall step inference."""

    vid: int
    pos_x: float
    pos_y: float
    direction: int


@dataclass(frozen=True)
class AxisCandidate:
    """A repeated same-VID coordinate delta."""

    vid: int
    dx: float
    dy: float
    count: int

    @property
    def abs_step_x(self) -> float:
        return abs(self.dx)

    @property
    def sign_name(self) -> str:
        if self.dx < 0:
            return "-x,+y"
        return "+x,+y"


@dataclass(frozen=True)
class OffsetCandidate:
    """A repeated sprite anchor offset from a pillar-based grid point."""

    offset_x: float
    offset_y: float
    count: int


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


def rounded_number(value: float) -> float:
    """Round map coordinates while preserving non-integer values."""
    rounded = round(value)
    if abs(value - rounded) <= FLOAT_TOLERANCE:
        return float(rounded)
    return round(value, 3)


def normalize_delta(first: SpritePoint, second: SpritePoint) -> tuple[float, float] | None:
    """Return a delta with positive Y so both pair orders collapse together."""
    dx = second.pos_x - first.pos_x
    dy = second.pos_y - first.pos_y

    dx = rounded_number(dx)
    dy = rounded_number(dy)

    if abs(dx) <= FLOAT_TOLERANCE:
        return None

    if abs(dy) <= FLOAT_TOLERANCE:
        return None

    if dy < 0:
        dx = -dx
        dy = -dy

    return dx, dy


def load_json(path: Path) -> dict:
    """Load unpacked map JSON."""
    text = path.read_text(encoding="utf-8")
    return json.loads(text)


def load_sprites(path: Path) -> list[SpritePoint]:
    """Load sprite points from an unpacked AS2 map JSON file."""
    data = load_json(path)
    sprite_section = data.get("SPR", {})
    raw_sprites = sprite_section.get("sprites", [])

    sprites: list[SpritePoint] = []
    for raw_sprite in raw_sprites:
        sprite = SpritePoint(
            vid=int(raw_sprite["vid"]),
            pos_x=float(raw_sprite["posX"]),
            pos_y=float(raw_sprite["posY"]),
            direction=int(raw_sprite.get("direction", 0)),
        )
        sprites.append(sprite)

    return sprites


def group_sprites_by_vid(sprites: list[SpritePoint]) -> dict[int, list[SpritePoint]]:
    """Group sprites by VID."""
    groups: dict[int, list[SpritePoint]] = {}

    for sprite in sprites:
        if sprite.vid not in groups:
            groups[sprite.vid] = []
        groups[sprite.vid].append(sprite)

    return groups


def collect_axis_candidates(sprites: list[SpritePoint]) -> list[AxisCandidate]:
    """Collect repeated same-VID deltas."""
    groups = group_sprites_by_vid(sprites)
    candidates: list[AxisCandidate] = []

    for vid, group in groups.items():
        delta_counts: dict[tuple[float, float], int] = {}

        for first_index in range(0, len(group)):
            first = group[first_index]
            for second_index in range(first_index + 1, len(group)):
                second = group[second_index]
                delta = normalize_delta(first, second)
                if delta is None:
                    continue

                if delta not in delta_counts:
                    delta_counts[delta] = 0
                delta_counts[delta] += 1

        for delta, count in delta_counts.items():
            if count < MIN_AXIS_REPEAT_COUNT:
                continue

            dx, dy = delta
            candidate = AxisCandidate(vid=vid, dx=dx, dy=dy, count=count)
            candidates.append(candidate)

    candidates.sort(key=axis_sort_key)
    return candidates


def axis_sort_key(candidate: AxisCandidate) -> tuple[int, float, int]:
    """Sort strongest, shortest candidates first."""
    distance = abs(candidate.dx) + abs(candidate.dy)
    return -candidate.count, distance, candidate.vid


def choose_best_axis_for_vid(candidates: list[AxisCandidate], vid: int, sign: int | None) -> AxisCandidate | None:
    """Choose the strongest axis candidate for one VID."""
    selected: list[AxisCandidate] = []

    for candidate in candidates:
        if candidate.vid != vid:
            continue

        if sign is not None:
            if sign < 0 and candidate.dx > 0:
                continue
            if sign > 0 and candidate.dx < 0:
                continue

        selected.append(candidate)

    if len(selected) == 0:
        return None

    selected.sort(key=axis_sort_key)
    return selected[0]


def infer_step_from_hints(
    candidates: list[AxisCandidate],
    dir_a_vids: set[int],
    dir_b_vids: set[int],
) -> tuple[float, float] | None:
    """Infer step from optional direction VID hints."""
    hinted_axes: list[AxisCandidate] = []

    for vid in dir_a_vids:
        axis = choose_best_axis_for_vid(candidates, vid, -1)
        if axis is not None:
            hinted_axes.append(axis)

    for vid in dir_b_vids:
        axis = choose_best_axis_for_vid(candidates, vid, 1)
        if axis is not None:
            hinted_axes.append(axis)

    if len(hinted_axes) == 0:
        return None

    return choose_step_from_axes(hinted_axes)


def choose_step_from_axes(candidates: list[AxisCandidate]) -> tuple[float, float] | None:
    """Choose the smallest repeated step with opposite-axis support."""
    step_scores: dict[tuple[float, float], int] = {}
    step_signs: dict[tuple[float, float], set[int]] = {}

    for candidate in candidates:
        step = (candidate.abs_step_x, candidate.dy)
        if step not in step_scores:
            step_scores[step] = 0
            step_signs[step] = set()
        step_scores[step] += candidate.count

        if candidate.dx < 0:
            step_signs[step].add(-1)
        else:
            step_signs[step].add(1)

    if len(step_scores) == 0:
        return None

    def step_sort_key(step: tuple[float, float]) -> tuple[float, int]:
        distance = step[0] + step[1]
        score = step_scores[step]
        return distance, -score

    opposite_axis_steps: list[tuple[float, float]] = []
    for step, signs in step_signs.items():
        if -1 in signs and 1 in signs:
            opposite_axis_steps.append(step)

    if len(opposite_axis_steps) > 0:
        opposite_axis_steps.sort(key=step_sort_key)
        return opposite_axis_steps[0]

    single_axis_steps: list[tuple[float, float]] = []
    for step in step_scores:
        single_axis_steps.append(step)

    single_axis_steps.sort(key=step_sort_key)
    return single_axis_steps[0]


def find_direction_vids(
    candidates: list[AxisCandidate],
    step: tuple[float, float],
    sign: int,
) -> list[AxisCandidate]:
    """Find VID candidates that match a selected step and axis sign."""
    matches: list[AxisCandidate] = []
    step_x, step_y = step

    for candidate in candidates:
        if abs(candidate.abs_step_x - step_x) > FLOAT_TOLERANCE:
            continue
        if abs(candidate.dy - step_y) > FLOAT_TOLERANCE:
            continue
        if sign < 0 and candidate.dx > 0:
            continue
        if sign > 0 and candidate.dx < 0:
            continue
        matches.append(candidate)

    matches.sort(key=axis_sort_key)
    return matches


def infer_pillar_vids(sprites: list[SpritePoint], direction_vids: set[int]) -> list[int]:
    """Return non-direction VIDs as pillar or corner candidates."""
    all_vids: set[int] = set()

    for sprite in sprites:
        all_vids.add(sprite.vid)

    pillar_vids: list[int] = []
    for vid in sorted(all_vids):
        if vid in direction_vids:
            continue
        pillar_vids.append(vid)

    return pillar_vids


def find_sprites_by_vids(sprites: list[SpritePoint], vids: set[int]) -> list[SpritePoint]:
    """Return sprites whose VID is in the selected set."""
    selected: list[SpritePoint] = []

    for sprite in sprites:
        if sprite.vid in vids:
            selected.append(sprite)

    return selected


def find_sprites_by_vid(sprites: list[SpritePoint], vid: int) -> list[SpritePoint]:
    """Return sprites with one VID."""
    selected: list[SpritePoint] = []

    for sprite in sprites:
        if sprite.vid == vid:
            selected.append(sprite)

    return selected


def offset_sort_key(candidate: OffsetCandidate) -> tuple[int, float]:
    """Sort strongest, smallest offsets first."""
    distance = abs(candidate.offset_x) + abs(candidate.offset_y)
    return -candidate.count, distance


def infer_axis_offset(
    wall_sprites: list[SpritePoint],
    pillar_sprites: list[SpritePoint],
    axis_dx: float,
    axis_dy: float,
    max_offset_x: float,
    max_offset_y: float,
    max_grid_steps: int,
) -> list[OffsetCandidate]:
    """Infer wall sprite offset from pillar-origin grid anchors."""
    offset_counts: dict[tuple[float, float], int] = {}

    if len(wall_sprites) == 0:
        return []

    if len(pillar_sprites) == 0:
        return []

    for pillar in pillar_sprites:
        for wall in wall_sprites:
            for step_index in range(1, max_grid_steps + 1):
                expected_x = pillar.pos_x + axis_dx * float(step_index)
                expected_y = pillar.pos_y + axis_dy * float(step_index)

                offset_x = rounded_number(wall.pos_x - expected_x)
                offset_y = rounded_number(wall.pos_y - expected_y)

                if abs(offset_x) > max_offset_x:
                    continue

                if abs(offset_y) >= max_offset_y:
                    continue

                offset = (offset_x, offset_y)

                if offset not in offset_counts:
                    offset_counts[offset] = 0
                offset_counts[offset] += 1

    candidates: list[OffsetCandidate] = []
    for offset, count in offset_counts.items():
        if count < MIN_OFFSET_REPEAT_COUNT:
            continue

        offset_x, offset_y = offset
        candidate = OffsetCandidate(
            offset_x=offset_x,
            offset_y=offset_y,
            count=count,
        )
        candidates.append(candidate)

    candidates.sort(key=offset_sort_key)
    return candidates


def format_offset_candidates(candidates: list[OffsetCandidate]) -> list[dict]:
    """Format offset candidates for JSON output."""
    items: list[dict] = []

    for candidate in candidates:
        item = {
            "offset_x": candidate.offset_x,
            "offset_y": candidate.offset_y,
            "repeat_count": candidate.count,
        }
        items.append(item)

    return items


def infer_offsets(
    sprites: list[SpritePoint],
    step: tuple[float, float],
    negative_axis: list[AxisCandidate],
    positive_axis: list[AxisCandidate],
    pillar_vids: list[int],
) -> dict:
    """Infer C++ WallProfile-style offsets."""
    step_x, step_y = step
    pillar_vid_set = set(pillar_vids)
    pillar_sprites = find_sprites_by_vids(sprites, pillar_vid_set)

    dir_a_reports: list[dict] = []
    for axis in negative_axis:
        wall_sprites = find_sprites_by_vid(sprites, axis.vid)
        candidates = infer_axis_offset(
            wall_sprites=wall_sprites,
            pillar_sprites=pillar_sprites,
            axis_dx=-step_x,
            axis_dy=step_y,
            max_offset_x=step_x,
            max_offset_y=step_y,
            max_grid_steps=MAX_OFFSET_GRID_STEPS,
        )
        dir_a_reports.append(
            {
                "vid": axis.vid,
                "offset_candidates": format_offset_candidates(candidates),
            }
        )

    dir_b_reports: list[dict] = []
    for axis in positive_axis:
        wall_sprites = find_sprites_by_vid(sprites, axis.vid)
        candidates = infer_axis_offset(
            wall_sprites=wall_sprites,
            pillar_sprites=pillar_sprites,
            axis_dx=step_x,
            axis_dy=step_y,
            max_offset_x=step_x,
            max_offset_y=step_y,
            max_grid_steps=MAX_OFFSET_GRID_STEPS,
        )
        dir_b_reports.append(
            {
                "vid": axis.vid,
                "offset_candidates": format_offset_candidates(candidates),
            }
        )

    return {
        "assumption": "pillar sprites are treated as grid anchors, so offset_p is fixed to (0, 0)",
        "offset_p_x": 0.0,
        "offset_p_y": 0.0,
        "dir_a_like_offsets": dir_a_reports,
        "dir_b_like_offsets": dir_b_reports,
    }


def build_report(
    path: Path,
    dir_a_vids: set[int],
    dir_b_vids: set[int],
    pillar_vids: set[int],
) -> dict:
    """Build an inference report for one unpacked JSON file."""
    sprites = load_sprites(path)
    candidates = collect_axis_candidates(sprites)

    step = infer_step_from_hints(candidates, dir_a_vids, dir_b_vids)
    if step is None:
        step = choose_step_from_axes(candidates)

    if step is None:
        return {
            "source": str(path),
            "sprite_count": len(sprites),
            "status": "failed",
            "reason": "No repeated same-VID isometric deltas were found.",
            "axis_candidates": [],
        }

    negative_axis = find_direction_vids(candidates, step, -1)
    positive_axis = find_direction_vids(candidates, step, 1)

    direction_vids: set[int] = set()
    for candidate in negative_axis:
        direction_vids.add(candidate.vid)
    for candidate in positive_axis:
        direction_vids.add(candidate.vid)

    inferred_pillar_vids = infer_pillar_vids(sprites, direction_vids)
    if len(pillar_vids) > 0:
        inferred_pillar_vids = sorted(pillar_vids)

    offset_report = infer_offsets(
        sprites=sprites,
        step=step,
        negative_axis=negative_axis,
        positive_axis=positive_axis,
        pillar_vids=inferred_pillar_vids,
    )

    axis_reports: list[dict] = []
    for candidate in candidates:
        axis_reports.append(
            {
                "vid": candidate.vid,
                "dx": candidate.dx,
                "dy": candidate.dy,
                "axis": candidate.sign_name,
                "repeat_count": candidate.count,
            }
        )

    return {
        "source": str(path),
        "sprite_count": len(sprites),
        "status": "ok",
        "step_x": step[0],
        "step_y": step[1],
        "dir_a_like_negative_x_vids": format_axis_matches(negative_axis),
        "dir_b_like_positive_x_vids": format_axis_matches(positive_axis),
        "pillar_or_corner_vid_candidates": inferred_pillar_vids,
        "offsets": offset_report,
        "axis_candidates": axis_reports,
    }


def format_axis_matches(candidates: list[AxisCandidate]) -> list[dict]:
    """Format axis matches for JSON output."""
    items: list[dict] = []

    for candidate in candidates:
        items.append(
            {
                "vid": candidate.vid,
                "dx": candidate.dx,
                "dy": candidate.dy,
                "repeat_count": candidate.count,
            }
        )

    return items


def collect_input_files(input_path: Path) -> list[Path]:
    """Collect JSON files from a file or directory input."""
    if input_path.is_file():
        return [input_path]

    files: list[Path] = []
    for path in input_path.rglob("*.json"):
        if path.name == REPORT_FILE_NAME:
            continue
        files.append(path)

    files.sort()
    return files


def print_report(report: dict) -> None:
    """Print a concise human-readable report."""
    print(f"Source: {report['source']}")
    print(f"Status: {report['status']}")

    if report["status"] != "ok":
        print(f"Reason: {report['reason']}")
        print()
        return

    print(f"Sprite count: {report['sprite_count']}")
    print(f"Step: {report['step_x']} x {report['step_y']}")
    print("Direction candidates:")

    print("  dir_a-like (/) (-x,+y):")
    print_axis_list(report["dir_a_like_negative_x_vids"])

    print("  dir_b-like (\) (+x,+y):")
    print_axis_list(report["dir_b_like_positive_x_vids"])

    print(f"Pillar/corner candidates: {report['pillar_or_corner_vid_candidates']}")
    print("Offsets:")
    print_offset_report(report["offsets"])
    print()


def print_axis_list(items: list[dict]) -> None:
    """Print axis candidate list."""
    if len(items) == 0:
        print("    none")
        return

    for item in items:
        print(
            f"    vid={item['vid']} delta=({item['dx']}, {item['dy']}) "
            f"repeat={item['repeat_count']}"
        )


def print_offset_report(offsets: dict) -> None:
    """Print inferred offset candidates."""
    print(f"  offset_p: ({offsets['offset_p_x']}, {offsets['offset_p_y']})")
    print(f"  assumption: {offsets['assumption']}")

    print("  dir_a-like offsets:")
    print_offset_axis_list(offsets["dir_a_like_offsets"])

    print("  dir_b-like offsets:")
    print_offset_axis_list(offsets["dir_b_like_offsets"])


def print_offset_axis_list(items: list[dict]) -> None:
    """Print offset candidates for one direction group."""
    if len(items) == 0:
        print("    none")
        return

    for item in items:
        vid = item["vid"]
        candidates = item["offset_candidates"]
        if len(candidates) == 0:
            print(f"    vid={vid}: none")
            continue

        best = candidates[0]
        print(
            f"    vid={vid}: best=({best['offset_x']}, {best['offset_y']}) "
            f"repeat={best['repeat_count']}"
        )


def parse_args() -> argparse.Namespace:
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Infer AS2 wall step sizes from unpacked 3x3 room JSON samples."
    )
    parser.add_argument(
        "input",
        type=Path,
        help="Unpacked map JSON file or a folder containing JSON files.",
    )
    parser.add_argument(
        "--dir-a-vid",
        default=None,
        help="Optional comma-separated VID hints for the -x,+y wall axis.",
    )
    parser.add_argument(
        "--dir-b-vid",
        default=None,
        help="Optional comma-separated VID hints for the +x,+y wall axis.",
    )
    parser.add_argument(
        "--pillar-vid",
        default=None,
        help="Optional comma-separated pillar/corner VID hints.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=None,
        help="Optional output report JSON path. Defaults to _wall_step_report.json beside folder input.",
    )
    return parser.parse_args()


def main() -> int:
    """CLI entry point."""
    args = parse_args()
    input_path = args.input

    if not input_path.exists():
        print(f"Error: input path does not exist: {input_path}")
        return 1

    dir_a_vids = parse_vid_list(args.dir_a_vid)
    dir_b_vids = parse_vid_list(args.dir_b_vid)
    pillar_vids = parse_vid_list(args.pillar_vid)

    files = collect_input_files(input_path)
    if len(files) == 0:
        print(f"Error: no JSON files found: {input_path}")
        return 1

    reports: list[dict] = []
    for file_path in files:
        report = build_report(file_path, dir_a_vids, dir_b_vids, pillar_vids)
        reports.append(report)
        print_report(report)

    output_path = args.output
    if output_path is None and input_path.is_dir():
        output_path = input_path / REPORT_FILE_NAME

    if output_path is not None:
        output_data = {
            "input": str(input_path),
            "reports": reports,
        }
        output_text = json.dumps(output_data, indent=2, ensure_ascii=False)
        output_path.write_text(output_text, encoding="utf-8")
        print(f"Saved report: {output_path}")

    failed_count = 0
    for report in reports:
        if report["status"] != "ok":
            failed_count += 1

    if failed_count > 0:
        return 2

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

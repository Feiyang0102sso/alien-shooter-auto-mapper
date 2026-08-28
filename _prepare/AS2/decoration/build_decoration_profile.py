"""从带参考墙的 AS2 地图生成装饰图章 profile 与 SVG 预览。

参考墙只用于拟合房间外框，不会写入图章成员。图章成员保留原始 SPR
数据，并以房间外框中心为原点记录平移偏移。
"""

import argparse
import json
import math
import sys
from pathlib import Path
from xml.sax.saxutils import escape


SCRIPT_DIR = Path(__file__).resolve().parent
AS2_TOOLS_DIR = SCRIPT_DIR.parent
sys.path.insert(0, str(AS2_TOOLS_DIR))

from as2_map_unpack import parse_map  # noqa: E402


WALL_STEP_X = 90.0
WALL_STEP_Y = 64.0
DEFAULT_LINE_TOLERANCE = 8.0
DEFAULT_PLACEHOLDER_WIDTH = 40.0
DEFAULT_PLACEHOLDER_HEIGHT = 28.0
SVG_MARGIN = 80.0


# AS2 全部九套墙。门和柱子也属于参考素材，选中对应墙组后会一并剔除。
WALL_PROFILES = (
    {
        "name": "AS2 Wall Set 1",
        "dir_a_vid": 1781,
        "dir_b_vid": 1780,
        "reference_vids": {1777, 1778, 1779, 1780, 1781, 1782, 1785, 1786, 1788},
    },
    {
        "name": "AS2 Wall Set 2",
        "dir_a_vid": 1701,
        "dir_b_vid": 1700,
        "reference_vids": {1700, 1701, 1702, 1703, 1785, 1786},
    },
    {
        "name": "AS2 Wall Set 3",
        "dir_a_vid": 1100,
        "dir_b_vid": 1101,
        "reference_vids": {1100, 1101, 1102, 1103, 1785, 1786},
    },
    {
        "name": "AS2 Wall Set 4",
        "dir_a_vid": 1131,
        "dir_b_vid": 1130,
        "reference_vids": {1103, 1130, 1131, 1132, 1785, 1786},
    },
    {
        "name": "AS2 Wall Set 5",
        "dir_a_vid": 2501,
        "dir_b_vid": 2500,
        "reference_vids": {631, 2500, 2501, 2502, 2504, 2505, 2506},
    },
    {
        "name": "AS2 Wall Set 6",
        "dir_a_vid": 2601,
        "dir_b_vid": 2600,
        "reference_vids": {2600, 2601, 2602, 2604, 2605, 2606},
    },
    {
        "name": "AS2 Wall Set 7",
        "dir_a_vid": 2621,
        "dir_b_vid": 2620,
        "reference_vids": {2620, 2621, 2622, 2624, 2645},
    },
    {
        "name": "AS2 Wall Set 8",
        "dir_a_vid": 2641,
        "dir_b_vid": 2640,
        "reference_vids": {2640, 2641, 2642, 2644, 2645},
    },
    {
        "name": "AS2 Wall Set 9",
        "dir_a_vid": 1721,
        "dir_b_vid": 1720,
        "reference_vids": {1720, 1721, 1731, 1732},
    },
)


def clean_float(value):
    """限制 JSON 小数噪声，同时消除负零。"""
    cleaned = round(float(value), 6)
    if cleaned == 0.0:
        return 0.0
    return cleaned


def parse_arguments():
    """读取命令行参数。"""
    parser = argparse.ArgumentParser(
        description="Build AS2 decoration stamp profiles from maps with reference walls."
    )
    parser.add_argument(
        "input_path",
        type=Path,
        help="AS2/AS2R .map source file, or a folder searched recursively for .map files",
    )
    parser.add_argument("--output", type=Path, help="Output profile JSON path")
    parser.add_argument("--preview", type=Path, help="Output SVG preview path")
    parser.add_argument(
        "--line-tolerance",
        type=float,
        default=DEFAULT_LINE_TOLERANCE,
        help="Maximum distance used to group slightly uneven wall anchors",
    )
    return parser.parse_args()


def count_vid(sprites, vid):
    """统计指定 VID 的精灵数量。"""
    count = 0
    for sprite in sprites:
        if sprite["vid"] == vid:
            count += 1
    return count


def candidate_score(candidate):
    """返回墙组候选的排序分数。"""
    return candidate["score"]


def get_normalized_intercept(line_point):
    """返回墙点的归一化直线截距。"""
    return line_point["normalized_intercept"]


def get_total_wall_length(cluster):
    """返回墙线分组的墙段总长度。"""
    return cluster["total_length"]


def get_raw_line_intercept(cluster):
    """返回墙线分组的原始直线截距。"""
    return cluster["raw_intercept"]


def select_wall_profile(sprites):
    """根据两种常规墙 VID 自动选择 AS2 墙组。"""
    candidates = []

    for profile in WALL_PROFILES:
        dir_a_count = count_vid(sprites, profile["dir_a_vid"])
        dir_b_count = count_vid(sprites, profile["dir_b_vid"])

        if dir_a_count < 2 or dir_b_count < 2:
            continue

        candidates.append(
            {
                "profile": profile,
                "dir_a_count": dir_a_count,
                "dir_b_count": dir_b_count,
                "score": dir_a_count + dir_b_count,
            }
        )

    if not candidates:
        raise ValueError("No complete AS2 wall set found in SPR records.")

    candidates.sort(key=candidate_score, reverse=True)
    best = candidates[0]

    if len(candidates) > 1:
        second = candidates[1]
        if best["score"] == second["score"]:
            raise ValueError(
                "Multiple AS2 wall sets have the same score; keep only one reference wall set."
            )

    return best


def calculate_line_intercept(sprite, direction_name):
    """把墙精灵锚点还原到网格线，并计算归一化直线截距。"""
    canonical_x = float(sprite["posX"])
    canonical_y = float(sprite["posY"])

    if direction_name == "dir_a":
        canonical_x -= WALL_STEP_X
        raw_intercept = WALL_STEP_Y * canonical_x + WALL_STEP_X * canonical_y
    else:
        raw_intercept = -WALL_STEP_Y * canonical_x + WALL_STEP_X * canonical_y

    normal_length = math.hypot(WALL_STEP_X, WALL_STEP_Y)
    normalized_intercept = raw_intercept / normal_length
    return raw_intercept, normalized_intercept


def cluster_wall_lines(wall_sprites, direction_name, tolerance):
    """将可能略有凹凸的墙锚点按共线距离分组。"""
    line_points = []

    for sprite in wall_sprites:
        raw_intercept, normalized_intercept = calculate_line_intercept(
            sprite, direction_name
        )
        line_points.append(
            {
                "raw_intercept": raw_intercept,
                "normalized_intercept": normalized_intercept,
            }
        )

    line_points.sort(key=get_normalized_intercept)
    clusters = []

    for point in line_points:
        if not clusters:
            clusters.append([point])
            continue

        current_cluster = clusters[-1]
        normalized_sum = 0.0
        for current_point in current_cluster:
            normalized_sum += current_point["normalized_intercept"]
        cluster_center = normalized_sum / len(current_cluster)

        distance = abs(point["normalized_intercept"] - cluster_center)
        if distance <= tolerance:
            current_cluster.append(point)
        else:
            clusters.append([point])

    results = []
    wall_length = math.hypot(WALL_STEP_X, WALL_STEP_Y)

    for cluster in clusters:
        raw_sum = 0.0
        normalized_sum = 0.0
        for point in cluster:
            raw_sum += point["raw_intercept"]
            normalized_sum += point["normalized_intercept"]

        count = len(cluster)
        results.append(
            {
                "raw_intercept": raw_sum / count,
                "normalized_intercept": normalized_sum / count,
                "wall_count": count,
                "total_length": count * wall_length,
            }
        )

    return results


def select_room_edges(clusters, direction_name):
    """按墙段总长度选取一个方向上最长的两条房间边。"""
    if len(clusters) < 2:
        raise ValueError(f"{direction_name} reference walls do not form two room edges.")

    clusters.sort(key=get_total_wall_length, reverse=True)

    if len(clusters) > 2:
        second_length = clusters[1]["total_length"]
        third_length = clusters[2]["total_length"]
        if math.isclose(second_length, third_length):
            raise ValueError(
                f"{direction_name} reference walls are ambiguous: "
                "the second and third runs have equal total length."
            )

    selected = [clusters[0], clusters[1]]
    selected.sort(key=get_raw_line_intercept)
    return selected


def intersect_edges(dir_a_edge, dir_b_edge):
    """求一条 / 墙线和一条反斜线墙线的交点。"""
    dir_a_intercept = dir_a_edge["raw_intercept"]
    dir_b_intercept = dir_b_edge["raw_intercept"]

    x = (dir_a_intercept - dir_b_intercept) / (2.0 * WALL_STEP_Y)
    y = (dir_a_intercept + dir_b_intercept) / (2.0 * WALL_STEP_X)
    return {"x": x, "y": y}


def calculate_frame(dir_a_edges, dir_b_edges):
    """由四条墙线求中心，再归一化为整数墙步长的封闭房间框。"""
    fitted_corners = []
    fitted_corners.append(intersect_edges(dir_a_edges[0], dir_b_edges[0]))
    fitted_corners.append(intersect_edges(dir_a_edges[0], dir_b_edges[1]))
    fitted_corners.append(intersect_edges(dir_a_edges[1], dir_b_edges[1]))
    fitted_corners.append(intersect_edges(dir_a_edges[1], dir_b_edges[0]))

    center_x = 0.0
    center_y = 0.0
    for corner in fitted_corners:
        center_x += corner["x"]
        center_y += corner["y"]
    center_x /= len(fitted_corners)
    center_y /= len(fitted_corners)

    # 原始四条线可能有轻微错位。墙素材只能按固定步长闭合，因此最终框
    # 使用两组最长墙线的整数墙数重新构造，保留拟合中心但不保留剪切误差。
    dir_a_wall_count = max(
        dir_a_edges[0]["wall_count"],
        dir_a_edges[1]["wall_count"],
    )
    dir_b_wall_count = max(
        dir_b_edges[0]["wall_count"],
        dir_b_edges[1]["wall_count"],
    )

    dir_a_vector_x = WALL_STEP_X * dir_a_wall_count
    dir_a_vector_y = -WALL_STEP_Y * dir_a_wall_count
    dir_b_vector_x = WALL_STEP_X * dir_b_wall_count
    dir_b_vector_y = WALL_STEP_Y * dir_b_wall_count

    corners = []
    corners.append(
        {
            "x": center_x + (dir_a_vector_x - dir_b_vector_x) / 2.0,
            "y": center_y + (dir_a_vector_y - dir_b_vector_y) / 2.0,
        }
    )
    corners.append(
        {
            "x": center_x - (dir_a_vector_x + dir_b_vector_x) / 2.0,
            "y": center_y - (dir_a_vector_y + dir_b_vector_y) / 2.0,
        }
    )
    corners.append(
        {
            "x": center_x + (-dir_a_vector_x + dir_b_vector_x) / 2.0,
            "y": center_y + (-dir_a_vector_y + dir_b_vector_y) / 2.0,
        }
    )
    corners.append(
        {
            "x": center_x + (dir_a_vector_x + dir_b_vector_x) / 2.0,
            "y": center_y + (dir_a_vector_y + dir_b_vector_y) / 2.0,
        }
    )

    side_lengths = []
    for index in range(len(corners)):
        next_index = (index + 1) % len(corners)
        delta_x = corners[next_index]["x"] - corners[index]["x"]
        delta_y = corners[next_index]["y"] - corners[index]["y"]
        side_length = math.hypot(delta_x, delta_y)
        side_lengths.append(side_length)

    for side_length in side_lengths:
        if side_length <= 0.0:
            raise ValueError("Reference walls produced a degenerate room frame.")

    minimum_x = corners[0]["x"]
    maximum_x = corners[0]["x"]
    minimum_y = corners[0]["y"]
    maximum_y = corners[0]["y"]

    for corner in corners[1:]:
        minimum_x = min(minimum_x, corner["x"])
        maximum_x = max(maximum_x, corner["x"])
        minimum_y = min(minimum_y, corner["y"])
        maximum_y = max(maximum_y, corner["y"])

    return {
        "center_x": center_x,
        "center_y": center_y,
        "corners": corners,
        "side_lengths": side_lengths,
        "bounds": {
            "minimum_x": minimum_x,
            "minimum_y": minimum_y,
            "maximum_x": maximum_x,
            "maximum_y": maximum_y,
        },
    }


def make_point(x, y):
    """创建二维点，不截断用于后续放置的坐标精度。"""
    return {"x": float(x), "y": float(y)}


def build_member(sprite, center_x, center_y):
    """复制一个装饰 SPR 的全部必要值，并计算相对中心偏移。"""
    rgba = sprite["gamma"]["rgba"]
    return {
        "vid": sprite["vid"],
        "source_x": sprite["posX"],
        "source_y": sprite["posY"],
        "offset_x": sprite["posX"] - center_x,
        "offset_y": sprite["posY"] - center_y,
        "pos_z": sprite["posZ"],
        "direction": sprite["direction"],
        "army": sprite["army"],
        "gamma": {
            "red": rgba[0],
            "green": rgba[1],
            "blue": rgba[2],
            "alpha": rgba[3],
        },
        "scale_x": sprite["scaleX"],
        "scale_y": sprite["scaleY"],
        "scale_z": sprite["scaleZ"],
    }


def build_profile(map_path, parsed_map, line_tolerance):
    """从已解析地图生成装饰图章 profile。"""
    version = parsed_map["HEAD"]["version"]["value"]
    if version not in ("0x00000013", "0x00000014"):
        raise ValueError(f"Unsupported map version: {version}")

    sprites = parsed_map["SPR"]["sprites"]
    selected = select_wall_profile(sprites)
    wall_profile = selected["profile"]

    dir_a_sprites = []
    dir_b_sprites = []
    for sprite in sprites:
        if sprite["vid"] == wall_profile["dir_a_vid"]:
            dir_a_sprites.append(sprite)
        if sprite["vid"] == wall_profile["dir_b_vid"]:
            dir_b_sprites.append(sprite)

    dir_a_clusters = cluster_wall_lines(dir_a_sprites, "dir_a", line_tolerance)
    dir_b_clusters = cluster_wall_lines(dir_b_sprites, "dir_b", line_tolerance)
    dir_a_edges = select_room_edges(dir_a_clusters, "dir_a")
    dir_b_edges = select_room_edges(dir_b_clusters, "dir_b")
    frame = calculate_frame(dir_a_edges, dir_b_edges)

    members = []
    reference_vids = wall_profile["reference_vids"]
    for sprite in sprites:
        if sprite["vid"] in reference_vids:
            continue
        member = build_member(sprite, frame["center_x"], frame["center_y"])
        members.append(member)

    if not members:
        raise ValueError("No decoration sprites remain after removing reference walls.")

    source_corners = []
    corner_offsets = []
    for corner in frame["corners"]:
        source_corners.append(make_point(corner["x"], corner["y"]))
        corner_offsets.append(
            make_point(
                corner["x"] - frame["center_x"],
                corner["y"] - frame["center_y"],
            )
        )

    side_lengths = []
    for side_length in frame["side_lengths"]:
        side_lengths.append(side_length)

    fitted_edges = {"dir_a": [], "dir_b": []}
    for edge in dir_a_edges:
        fitted_edges["dir_a"].append(
            {
                "wall_count": edge["wall_count"],
                "total_length": edge["total_length"],
            }
        )
    for edge in dir_b_edges:
        fitted_edges["dir_b"].append(
            {
                "wall_count": edge["wall_count"],
                "total_length": edge["total_length"],
            }
        )

    bounds = frame["bounds"]
    return {
        "schema_version": 1,
        "stamp_id": map_path.stem,
        "source_file": map_path.name,
        "source_format": parsed_map["HEAD"]["version"]["name"],
        "wall_profile": wall_profile["name"],
        "frame": {
            "source_center_x": frame["center_x"],
            "source_center_y": frame["center_y"],
            "source_corners": source_corners,
            "corner_offsets": corner_offsets,
            "side_lengths": side_lengths,
            "axis_aligned_bounds": {
                "minimum_x": bounds["minimum_x"],
                "minimum_y": bounds["minimum_y"],
                "maximum_x": bounds["maximum_x"],
                "maximum_y": bounds["maximum_y"],
            },
        },
        "wall_fit": {
            "line_tolerance": line_tolerance,
            "selected_edges": fitted_edges,
        },
        "preview": {
            "default_placeholder_width": DEFAULT_PLACEHOLDER_WIDTH,
            "default_placeholder_height": DEFAULT_PLACEHOLDER_HEIGHT,
        },
        "member_count": len(members),
        "members": members,
        "test_boundary": {
            "dir_a_vid": wall_profile["dir_a_vid"],
            "dir_b_vid": wall_profile["dir_b_vid"],
            "step_x": WALL_STEP_X,
            "step_y": WALL_STEP_Y,
        },
    }


def svg_number(value):
    """将数值格式化为简洁 SVG 属性。"""
    return f"{clean_float(value):g}"


def write_svg_preview(profile, output_path):
    """绘制大框、成员占位小框和成员锚点。"""
    frame = profile["frame"]
    bounds = frame["axis_aligned_bounds"]
    minimum_x = bounds["minimum_x"] - SVG_MARGIN
    minimum_y = bounds["minimum_y"] - SVG_MARGIN
    maximum_x = bounds["maximum_x"] + SVG_MARGIN
    maximum_y = bounds["maximum_y"] + SVG_MARGIN

    width = maximum_x - minimum_x
    height = maximum_y - minimum_y
    if width <= 0.0 or height <= 0.0:
        raise ValueError("Cannot draw an empty SVG preview.")

    polygon_points = []
    for corner in frame["source_corners"]:
        polygon_points.append(
            f'{svg_number(corner["x"])},{svg_number(corner["y"])}'
        )

    svg_lines = []
    svg_lines.append('<?xml version="1.0" encoding="UTF-8"?>')
    svg_lines.append(
        '<svg xmlns="http://www.w3.org/2000/svg" '
        f'viewBox="{svg_number(minimum_x)} {svg_number(minimum_y)} '
        f'{svg_number(width)} {svg_number(height)}">'
    )
    svg_lines.append("  <style>")
    svg_lines.append("    .frame { fill: #19202a; fill-opacity: 0.13; stroke: #ff4b4b; stroke-width: 4; }")
    svg_lines.append("    .member { fill: #8ea4b8; fill-opacity: 0.25; stroke: #d7e3ec; stroke-width: 1.5; }")
    svg_lines.append("    .anchor { fill: #53e0c1; }")
    svg_lines.append("    .label { fill: #ffffff; font: 12px sans-serif; paint-order: stroke; stroke: #111820; stroke-width: 3px; }")
    svg_lines.append("    .center { fill: #ffd166; stroke: #111820; stroke-width: 2; }")
    svg_lines.append("  </style>")
    svg_lines.append(
        f'  <rect x="{svg_number(minimum_x)}" y="{svg_number(minimum_y)}" '
        f'width="{svg_number(width)}" height="{svg_number(height)}" '
        'fill="#080b10" />'
    )
    svg_lines.append(f'  <polygon class="frame" points="{" ".join(polygon_points)}" />')

    placeholder_width = profile["preview"]["default_placeholder_width"]
    placeholder_height = profile["preview"]["default_placeholder_height"]
    half_width = placeholder_width / 2.0
    half_height = placeholder_height / 2.0

    for member in profile["members"]:
        x = member["source_x"] - half_width
        y = member["source_y"] - half_height
        svg_lines.append(
            '  <rect class="member" '
            f'x="{svg_number(x)}" y="{svg_number(y)}" '
            f'width="{svg_number(placeholder_width)}" '
            f'height="{svg_number(placeholder_height)}" />'
        )
        svg_lines.append(
            f'  <circle class="anchor" cx="{svg_number(member["source_x"])}" '
            f'cy="{svg_number(member["source_y"])}" r="3" />'
        )
        label = escape(f'{member["vid"]}/{member["direction"]}')
        label_x = member["source_x"] + half_width + 3.0
        label_y = member["source_y"] - 3.0
        svg_lines.append(
            f'  <text class="label" x="{svg_number(label_x)}" '
            f'y="{svg_number(label_y)}">{label}</text>'
        )

    svg_lines.append(
        f'  <circle class="center" cx="{svg_number(frame["source_center_x"])}" '
        f'cy="{svg_number(frame["source_center_y"])}" r="6" />'
    )
    svg_lines.append("</svg>")

    output_path.write_text("\n".join(svg_lines) + "\n", encoding="utf-8")


def collect_map_files(input_path):
    """把输入路径展开成待处理的 .map 文件列表。"""
    if input_path.is_file():
        if input_path.suffix.lower() != ".map":
            raise ValueError(f"Expected a .map file: {input_path}")
        return [input_path]

    if not input_path.is_dir():
        raise FileNotFoundError(f"Input path not found: {input_path}")

    map_files = []
    for candidate in sorted(input_path.rglob("*.map")):
        if candidate.is_file():
            map_files.append(candidate)

    if not map_files:
        raise FileNotFoundError(f"No .map file found in folder: {input_path}")

    return map_files


def build_one_map(map_path, output_path, preview_path, line_tolerance):
    """处理单个地图文件，写出 profile 与 SVG 预览。"""
    parsed_map = parse_map(map_path)
    profile = build_profile(map_path, parsed_map, line_tolerance)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    preview_path.parent.mkdir(parents=True, exist_ok=True)
    output_text = json.dumps(profile, indent=2, ensure_ascii=False)
    output_path.write_text(output_text + "\n", encoding="utf-8")
    write_svg_preview(profile, preview_path)

    print(f"[INFO] Map: {map_path.name}")
    print(f'[INFO] Wall profile: {profile["wall_profile"]}')
    print(f'[INFO] Decoration members: {profile["member_count"]}')
    print(f"[INFO] Profile saved: {output_path}")
    print(f"[INFO] Preview saved: {preview_path}")


def main():
    """命令行入口。"""
    arguments = parse_arguments()
    input_path = arguments.input_path.resolve()

    if arguments.line_tolerance < 0.0:
        raise ValueError("Line tolerance must not be negative.")

    map_files = collect_map_files(input_path)

    if len(map_files) > 1:
        if arguments.output is not None or arguments.preview is not None:
            raise ValueError(
                "--output/--preview only work with a single .map file input."
            )

    failed_maps = []
    for map_path in map_files:
        output_path = arguments.output
        if output_path is None:
            output_path = map_path.with_suffix(".decoration.json")

        preview_path = arguments.preview
        if preview_path is None:
            preview_path = map_path.with_suffix(".decoration.svg")

        try:
            build_one_map(
                map_path,
                output_path.resolve(),
                preview_path.resolve(),
                arguments.line_tolerance,
            )
        except (ValueError, OSError) as error:
            # 批量处理时单个地图失败不该中断其余地图，先记录，最后统一汇总。
            print(f"[ERROR] {map_path.name}: {error}", file=sys.stderr)
            failed_maps.append(map_path)

    converted_count = len(map_files) - len(failed_maps)
    print(f"[INFO] Done: {converted_count}/{len(map_files)} map files converted.")

    if failed_maps:
        raise ValueError(f"{len(failed_maps)} map file(s) failed.")


if __name__ == "__main__":
    try:
        main()
    except (FileNotFoundError, ValueError, OSError) as error:
        print(f"[ERROR] {error}", file=sys.stderr)
        sys.exit(1)

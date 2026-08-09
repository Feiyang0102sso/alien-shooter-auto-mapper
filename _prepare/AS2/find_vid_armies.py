"""批量查询 AS2 地图中各 VID 对应的默认 Army。

用法:
    python find_vid_armies.py <map_or_folder> [more_inputs ...]
    python find_vid_armies.py <map_or_folder> --output <txt_file>
"""

import argparse
import contextlib
import io
import re
import struct
import sys
from pathlib import Path

from as2_map_unpack import parse_map


SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_INI_DIR = SCRIPT_DIR / "ini"
DEFAULT_OUTPUT_PATH = SCRIPT_DIR / "_vid_army_report.txt"
MAP_SUFFIX = ".map"
WEAPON_BLOCK_PATTERN = re.compile(r"^;-+(\d+)$")


def parse_integer_assignment(line, key):
    """解析形如 Key=123 的整数字段。"""
    prefix = f"{key}="
    if not line.startswith(prefix):
        return None

    value_text = line[len(prefix):].strip()
    return int(value_text)


def load_vid_weapon_index(obj_path):
    """从 OBJ.ini 建立 VID 到武器编号的索引。"""
    vid_weapon_index = {}
    current_vid = None

    with obj_path.open("r", encoding="latin-1") as obj_file:
        for raw_line in obj_file:
            line = raw_line.strip()

            vid = parse_integer_assignment(line, "NVid")
            if vid is not None:
                current_vid = vid
                continue

            weapon = parse_integer_assignment(line, "NWeapon")
            if weapon is None:
                continue
            if current_vid is None:
                continue

            vid_weapon_index[current_vid] = weapon

    return vid_weapon_index


def load_weapon_army_index(weap_path):
    """从 WEAP.ini 建立武器编号到 DefaultArmy 的索引。"""
    weapon_army_index = {}
    current_weapon = None

    with weap_path.open("r", encoding="latin-1") as weap_file:
        for raw_line in weap_file:
            line = raw_line.strip()
            block_match = WEAPON_BLOCK_PATTERN.fullmatch(line)

            if block_match is not None:
                current_weapon = int(block_match.group(1))
                continue

            default_army = parse_integer_assignment(line, "DefaultArmy")
            if default_army is None:
                continue
            if current_weapon is None:
                continue

            weapon_army_index[current_weapon] = default_army

    return weapon_army_index


def path_sort_key(path):
    """提供不区分大小写的稳定路径排序。"""
    return str(path).casefold()


def collect_map_paths(input_values):
    """展开文件和文件夹参数，返回去重后的地图路径。"""
    map_paths = []
    known_paths = set()

    for input_value in input_values:
        input_path = Path(input_value).expanduser()
        if not input_path.exists():
            raise FileNotFoundError(f"Input path not found: {input_path}")

        candidates = []
        if input_path.is_file():
            candidates.append(input_path)
        else:
            for child_path in input_path.rglob("*"):
                if child_path.is_file():
                    candidates.append(child_path)

        for candidate in candidates:
            if candidate.suffix.casefold() != MAP_SUFFIX:
                continue

            resolved_path = candidate.resolve()
            if resolved_path in known_paths:
                continue

            known_paths.add(resolved_path)
            map_paths.append(resolved_path)

    map_paths.sort(key=path_sort_key)
    if len(map_paths) == 0:
        raise ValueError("No .map files found in the supplied inputs.")

    return map_paths


def extract_unique_vids(map_path):
    """复用地图解包器，提取地图内不重复的 VID。"""
    # 原解包器会输出完整解析日志；批处理报告只保留本工具的简短进度。
    parser_output = io.StringIO()
    with contextlib.redirect_stdout(parser_output):
        map_data = parse_map(map_path)

    sprites = map_data["SPR"]["sprites"]
    unique_vids = set()
    for sprite in sprites:
        unique_vids.add(sprite["vid"])

    return sorted(unique_vids)


def display_path(map_path):
    """优先使用相对脚本目录的路径，保留重名地图的区分信息。"""
    try:
        relative_path = map_path.relative_to(SCRIPT_DIR)
        return str(relative_path)
    except ValueError:
        return str(map_path)


def append_map_report(lines, map_path, vids, vid_weapon_index, weapon_army_index):
    """追加一张地图的 VID、武器编号和默认 Army。"""
    lines.append(f"[{display_path(map_path)}]")
    lines.append(f"VID count: {len(vids)}")
    lines.append("VID\tNWeapon\tDefaultArmy")

    for vid in vids:
        weapon = vid_weapon_index.get(vid)
        if weapon is None:
            lines.append(f"{vid}\tNOT_FOUND\tNOT_FOUND")
            continue

        default_army = weapon_army_index.get(weapon)
        if default_army is None:
            lines.append(f"{vid}\t{weapon}\tNOT_FOUND")
            continue

        lines.append(f"{vid}\t{weapon}\t{default_army}")

    lines.append("")


def write_report(map_paths, output_path, vid_weapon_index, weapon_army_index):
    """解析所有地图并写出汇总报告，返回失败地图数量。"""
    report_lines = ["AS2 VID DefaultArmy Report", ""]
    failed_map_count = 0

    for map_path in map_paths:
        print(f"Processing: {map_path}")
        try:
            vids = extract_unique_vids(map_path)
        except (OSError, ValueError, struct.error) as error:
            failed_map_count += 1
            report_lines.append(f"[{display_path(map_path)}]")
            report_lines.append(f"ERROR: {error}")
            report_lines.append("")
            print(f"Error: {map_path}: {error}", file=sys.stderr)
            continue

        append_map_report(
            report_lines,
            map_path,
            vids,
            vid_weapon_index,
            weapon_army_index,
        )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    report_text = "\n".join(report_lines)
    output_path.write_text(report_text, encoding="utf-8-sig")
    return failed_map_count


def build_argument_parser():
    """创建命令行参数解析器。"""
    argument_parser = argparse.ArgumentParser(
        description="Find the default Army for every VID used by AS2 map files."
    )
    argument_parser.add_argument(
        "inputs",
        nargs="+",
        help="One or more .map files or folders. Folders are scanned recursively.",
    )
    argument_parser.add_argument(
        "--output",
        type=Path,
        default=DEFAULT_OUTPUT_PATH,
        help=f"Output TXT path. Default: {DEFAULT_OUTPUT_PATH}",
    )
    return argument_parser


def main():
    """命令行入口。"""
    argument_parser = build_argument_parser()
    arguments = argument_parser.parse_args()

    obj_path = DEFAULT_INI_DIR / "OBJ.ini"
    weap_path = DEFAULT_INI_DIR / "WEAP.ini"
    if not obj_path.is_file():
        argument_parser.error(f"OBJ.ini not found: {obj_path}")
    if not weap_path.is_file():
        argument_parser.error(f"WEAP.ini not found: {weap_path}")

    try:
        map_paths = collect_map_paths(arguments.inputs)
    except (FileNotFoundError, ValueError) as error:
        argument_parser.error(str(error))

    print(f"Loading: {obj_path}")
    vid_weapon_index = load_vid_weapon_index(obj_path)
    print(f"Loading: {weap_path}")
    weapon_army_index = load_weapon_army_index(weap_path)

    output_path = arguments.output.expanduser().resolve()
    failed_map_count = write_report(
        map_paths,
        output_path,
        vid_weapon_index,
        weapon_army_index,
    )

    print(f"Report saved: {output_path}")
    print(f"Maps processed: {len(map_paths) - failed_map_count}/{len(map_paths)}")
    if failed_map_count > 0:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())

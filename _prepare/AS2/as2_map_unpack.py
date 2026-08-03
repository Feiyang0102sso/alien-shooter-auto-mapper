"""
Alien Shooter 2 / Zombie Shooter 2 map 文件解包脚本。

根据 AS2_MAP.bt 解析 RES/MAP 容器，输出结构化 JSON。
当前脚本重点覆盖 AS2 legacy version 0x13 与 AS2R/AS2C/ZS2 version 0x14。

用法:
    python as2_map_unpack.py <map_file> [--output <json_file>]
"""

import json
import struct
import sys
from pathlib import Path


MAGIC_RES = b"RES "
MAGIC_MAP = b"MAP "
MAGIC_GRPH = b"GRPH"
MAGIC_HEAD = b"HEAD"
MAGIC_SPR = b"SPR "
MAGIC_SPRD = b"SPRD"
MAGIC_PLAY = b"PLAY"
MAGIC_GROU = b"GROU"

SPRITE_RECORD_SIZE = 48
SINGLE_COMMAND_SIZE = 16
TERMINATOR = 0xFFFFFFFF

COMMAND_ID_NAMES = {
    0: "ANI_STAND",
    1: "ANI_STOP_MOVE_ANI_BUILD",
    2: "ANI_GO",
    3: "ANI_START_MOVE",
    4: "ANI_L_ROTATE",
    5: "ANI_R_ROTATE",
    6: "ANI_OPEN",
    7: "ANI_HIT",
    8: "ANI_FIGHT",
    9: "ANI_SALUT",
    10: "ANI_STAND_OPEN",
    11: "ANI_CLASH_VERT",
    12: "ANI_LAZY_ANI_CLASH",
    13: "ANI_WOUND",
    14: "ANI_BIRTH",
    15: "ANI_DEATH",
    16: "ANI_DEATH2",
    32: "ACT_ATTACK",
    33: "ACT_MOVE",
    34: "ACT_MOVE_TO",
    35: "ACT_BUILD_UNIT",
    36: "ACT_PATROL",
    37: "ACT_COOR_ATTACK",
    38: "ACT_RANDOM",
    39: "ACT_STOP",
    40: "ACT_PAUSE",
    41: "ACT_ROTATE",
    43: "ACT_FLAGMAN_TRIGGER",
    44: "ACT_MOVE_ACCURATELY",
    45: "ACT_FOLLOW",
    46: "ACT_STAND_FIRM",
    62: "ACT_CHANGE_VID",
    63: "ACT_CHANGE_COOR",
    71: "ACT_GOTO_STACK",
    72: "ACT_CLEAR_STACK",
    73: "ACT_STOP_STACK",
    77: "ACT_WHILE_NOT_QUEST",
    78: "ACT_SET_QUEST",
    79: "ACT_WHILE_NOT_SCRIPT_VAR",
    85: "ACT_DAMAGE",
    88: "ACT_SET_HP",
    91: "ACT_SET_GOAL_FOR_TELEPORT",
    95: "ACT_SET_BEHAVE",
    97: "ACT_SET_ARMY",
    98: "ACT_SET_INVISIBLE",
    102: "ACT_SET_LINK",
    104: "ACT_SET_UPLINK",
    113: "ACT_SET_DEATH_TIMER",
    115: "ACT_SET_DIALOG",
    116: "ACT_SET_FLAGMAN_DIALOG",
    131: "ACT_SCRIPT_VAR",
    134: "ACT_DESTROY_UNIT",
    135: "ACT_PLAY_SFX",
    136: "ACT_SCRIPT_EVENT",
}


def read_uint32(data, offset):
    """读取小端 uint32。"""
    return struct.unpack_from("<I", data, offset)[0]


def read_int32(data, offset):
    """读取小端 int32。"""
    return struct.unpack_from("<i", data, offset)[0]


def read_float(data, offset):
    """读取小端 float。"""
    return struct.unpack_from("<f", data, offset)[0]


def read_magic(data, offset, expected):
    """读取并校验 4 字节魔数。"""
    actual = data[offset:offset + 4]
    if actual != expected:
        raise ValueError(
            f"Magic mismatch at 0x{offset:X}: expected {expected!r}, got {actual!r}"
        )
    return actual.decode("ascii")


def hex_uint32(value):
    """按 010 Editor 习惯显示 uint32 十六进制。"""
    return f"0x{value:08X}"


def decode_c_string(raw_bytes):
    """解码 BT 中 nameText 一类的 null-terminated 字符串。"""
    text_bytes = raw_bytes
    if len(raw_bytes) > 0 and raw_bytes[-1] == 0:
        text_bytes = raw_bytes[:-1]
    return text_bytes.decode("utf-8", errors="replace")


def format_map_version(value):
    """返回 AS2 地图版本说明。"""
    if value == 0x13:
        return "AS2 legacy"
    if value == 0x14:
        return "AS2R/AS2C/ZS2"
    return "Unknown"


def format_environment(value):
    """解析 GRPH 环境标记。"""
    flags = []
    if value & 0x00000001:
        flags.append("Shadow")
    if value & 0x00000C00:
        flags.append("Rain")
    if value & 0x0000C000:
        flags.append("Snow")
    if value & 0x00010000:
        flags.append("Sunlight")

    if not flags:
        return "None"
    return " ".join(flags)


def map_gamma_ui_value(lower_value, upper_value):
    """把地图 gamma 存储值还原为编辑器 UI 值。"""
    if upper_value > 0:
        return 255 - upper_value // 2
    if lower_value > 1:
        return lower_value // 2
    return 0


def parse_map_gamma(raw_bytes):
    """解析 8 字节地图 gamma。"""
    lower_blue = raw_bytes[0]
    lower_green = raw_bytes[1]
    lower_red = raw_bytes[2]
    upper_marker = raw_bytes[3]
    upper_blue = raw_bytes[4]
    upper_green = raw_bytes[5]
    upper_red = raw_bytes[6]
    reserved = raw_bytes[7]

    red = map_gamma_ui_value(lower_red, upper_red)
    green = map_gamma_ui_value(lower_green, upper_green)
    blue = map_gamma_ui_value(lower_blue, upper_blue)

    return {
        "lowerBlue": lower_blue,
        "lowerGreen": lower_green,
        "lowerRed": lower_red,
        "upperMarker": upper_marker,
        "upperBlue": upper_blue,
        "upperGreen": upper_green,
        "upperRed": upper_red,
        "reserved": reserved,
        "uiRGB": [red, green, blue],
    }


def parse_vid_gamma(raw_bytes):
    """解析 8 字节单位 RGBA gamma。"""
    negative_blue = raw_bytes[0]
    negative_green = raw_bytes[1]
    negative_red = raw_bytes[2]
    negative_alpha = raw_bytes[3]
    positive_blue = raw_bytes[4]
    positive_green = raw_bytes[5]
    positive_red = raw_bytes[6]
    positive_alpha = raw_bytes[7]

    red = positive_red
    green = positive_green
    blue = positive_blue
    alpha = positive_alpha

    if negative_red > 0:
        red = -negative_red
    if negative_green > 0:
        green = -negative_green
    if negative_blue > 0:
        blue = -negative_blue
    if negative_alpha > 0:
        alpha = -negative_alpha

    return {
        "negativeBlue": negative_blue,
        "negativeGreen": negative_green,
        "negativeRed": negative_red,
        "negativeAlpha": negative_alpha,
        "positiveBlue": positive_blue,
        "positiveGreen": positive_green,
        "positiveRed": positive_red,
        "positiveAlpha": positive_alpha,
        "rgba": [red, green, blue, alpha],
    }


def format_behavior(value):
    """解析 AS2 行为标记。"""
    flags = []
    if value & 0x00000001:
        flags.append("Aggressive")
    if value & 0x00000002:
        flags.append("Active")
    if value & 0x00000004:
        flags.append("RandomMove")
    if value & 0x00010000:
        flags.append("InheritStackAfterDeath")

    if not flags:
        return "None"
    return " ".join(flags)


def get_command_name(action_id):
    """返回命令 ID 的可读名称。"""
    name = COMMAND_ID_NAMES.get(action_id)
    if name is not None:
        return name
    return f"UNKNOWN_COMMAND_{action_id}"


def parse_file_header(data, offset):
    """解析 RES/MAP 文件头。"""
    read_magic(data, offset, MAGIC_RES)
    map_size = read_uint32(data, offset + 4)
    read_magic(data, offset + 8, MAGIC_MAP)

    result = {
        "RES_Magic": "RES ",
        "mapSize": map_size,
        "MAP_Magic": "MAP ",
    }
    print(f"[FileHeader] mapSize={map_size}")
    return result, offset + 12


def parse_grph(data, offset):
    """解析 GRPH 图形环境块。"""
    read_magic(data, offset, MAGIC_GRPH)
    graph_size = read_uint32(data, offset + 4)
    options = read_uint32(data, offset + 8)
    packed_diff = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)
    graph_info_size = read_uint32(data, offset + 20)

    cursor = offset + 24
    graph_info_start = cursor

    environment = read_uint32(data, cursor)
    cursor += 4
    gamma = parse_map_gamma(data[cursor:cursor + 8])
    cursor += 8
    wind_direction = read_uint32(data, cursor)
    cursor += 4
    wind_speed = read_float(data, cursor)
    cursor += 4

    result = {
        "GRPH_Magic": "GRPH",
        "graphSize": graph_size,
        "options": hex_uint32(options),
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "graphInfoSize": graph_info_size,
        "environment": {
            "value": hex_uint32(environment),
            "flags": format_environment(environment),
        },
        "gamma": gamma,
        "windDirection": wind_direction,
        "windSpeed": wind_speed,
    }

    if graph_info_size >= 0x20:
        sun_gamma = read_uint32(data, cursor)
        cursor += 4
        sun_property_1 = read_uint32(data, cursor)
        cursor += 4
        sun_property_2 = read_uint32(data, cursor)
        cursor += 4
        result["sun"] = {
            "sunGamma": hex_uint32(sun_gamma),
            "sunProperty1": sun_property_1,
            "sunProperty2": sun_property_2,
        }

    if graph_info_size >= 0x38:
        result["reserved"] = data[cursor:cursor + 24].hex()
        cursor += 24

    consumed = cursor - graph_info_start
    if consumed < graph_info_size:
        tail_size = graph_info_size - consumed
        result["graphTail"] = data[cursor:cursor + tail_size].hex()
        cursor += tail_size

    print(
        f"[GRPH] graphSize={graph_size}, graphInfoSize=0x{graph_info_size:X}, "
        f"environment={format_environment(environment)}"
    )
    return result, offset + 8 + graph_size


def parse_head(data, offset):
    """解析 HEAD 地图基础信息块。"""
    read_magic(data, offset, MAGIC_HEAD)
    head_size = read_uint32(data, offset + 4)
    options = read_uint32(data, offset + 8)
    packed_diff = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)
    map_info_size = read_uint32(data, offset + 20)
    map_size_x = read_float(data, offset + 24)
    map_size_y = read_float(data, offset + 28)
    map_camera_x = read_float(data, offset + 32)
    map_camera_y = read_float(data, offset + 36)
    current_time = read_uint32(data, offset + 40)
    version = read_int32(data, offset + 44)

    result = {
        "HEAD_Magic": "HEAD",
        "headSize": head_size,
        "options": hex_uint32(options),
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "mapInfoSize": map_info_size,
        "mapSizeX": map_size_x,
        "mapSizeY": map_size_y,
        "mapCameraX": map_camera_x,
        "mapCameraY": map_camera_y,
        "currentTime": current_time,
        "version": {
            "value": hex_uint32(version),
            "name": format_map_version(version),
        },
    }
    print(
        f"[HEAD] mapSize=({map_size_x}, {map_size_y}), "
        f"version=0x{version:X} {format_map_version(version)}"
    )
    return result, offset + 8 + head_size


def parse_spr(data, offset):
    """解析 SPR 精灵基础记录块。"""
    read_magic(data, offset, MAGIC_SPR)
    spr_size = read_uint32(data, offset + 4)
    options = read_uint32(data, offset + 8)
    packed_diff = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)
    spr_info_size = read_uint32(data, offset + 20)

    sprite_count = (spr_info_size - 4) // SPRITE_RECORD_SIZE
    cursor = offset + 24
    sprites = []

    for _ in range(sprite_count):
        sprite = {
            "objectId": read_uint32(data, cursor),
            "vid": read_int32(data, cursor + 4),
            "posX": read_float(data, cursor + 8),
            "posY": read_float(data, cursor + 12),
            "posZ": read_float(data, cursor + 16),
            "direction": read_uint32(data, cursor + 20),
            "army": read_int32(data, cursor + 24),
            "gamma": parse_vid_gamma(data[cursor + 28:cursor + 36]),
            "scaleX": read_float(data, cursor + 36),
            "scaleY": read_float(data, cursor + 40),
            "scaleZ": read_float(data, cursor + 44),
        }
        sprites.append(sprite)
        cursor += SPRITE_RECORD_SIZE

    terminator = read_uint32(data, cursor)

    result = {
        "SPR_Magic": "SPR ",
        "sprSize": spr_size,
        "options": hex_uint32(options),
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "sprInfoSize": spr_info_size,
        "spriteRecordSize": SPRITE_RECORD_SIZE,
        "spriteCount": sprite_count,
        "sprites": sprites,
        "terminator": hex_uint32(terminator),
    }
    print(f"[SPR] spriteCount={sprite_count}, sprInfoSize={spr_info_size}")
    return result, offset + 8 + spr_size


def parse_command_stack_block(data, offset, remaining):
    """按 AS2 的 stackCount/slot/cmd 布局解析命令栈。"""
    stack_count = read_uint32(data, offset)
    cursor = offset + 4
    consumed = 4
    slots = []

    if stack_count > 4:
        return None, offset, 0

    for _ in range(stack_count):
        if consumed + 4 > remaining:
            return None, offset, 0

        command_count = read_uint32(data, cursor)
        cursor += 4
        consumed += 4

        command_bytes = command_count * SINGLE_COMMAND_SIZE
        if consumed + command_bytes > remaining:
            return None, offset, 0

        commands = []
        for _ in range(command_count):
            action_id = read_uint32(data, cursor)
            command = {
                "actionId": action_id,
                "actionName": get_command_name(action_id),
                "param1": read_uint32(data, cursor + 4),
                "param2": read_uint32(data, cursor + 8),
                "param3": read_uint32(data, cursor + 12),
            }
            commands.append(command)
            cursor += SINGLE_COMMAND_SIZE
            consumed += SINGLE_COMMAND_SIZE

        slots.append({
            "cmdCount": command_count,
            "commands": commands,
        })

    result = {
        "stackCount": stack_count,
        "slots": slots,
    }
    return result, cursor, consumed


def parse_drop_list(data, offset, remaining):
    """解析掉落列表。"""
    if remaining < 4:
        return None, offset, 0

    item_count = read_uint32(data, offset)
    drop_list_size = 4 + item_count * 4
    if drop_list_size > remaining:
        return None, offset, 0

    cursor = offset + 4
    item_vids = []
    for _ in range(item_count):
        item_vids.append(read_uint32(data, cursor))
        cursor += 4

    result = {
        "itemCount": item_count,
        "itemVids": item_vids,
    }
    return result, cursor, drop_list_size


def parse_sprd_sub(data, offset):
    """解析单个 SPRD 子块。"""
    sub_size = read_uint32(data, offset)
    cursor = offset + 4

    if sub_size == 4:
        terminator = read_uint32(data, cursor)
        result = {
            "subSize": sub_size,
            "isTerminator": True,
            "terminator": hex_uint32(terminator),
        }
        return result, cursor + 4

    payload_start = cursor
    object_id = read_uint32(data, cursor)
    cursor += 4
    remaining = sub_size - 4

    result = {
        "subSize": sub_size,
        "isTerminator": False,
        "objectId": object_id,
    }

    if remaining >= 4:
        behavior = read_uint32(data, cursor)
        cursor += 4
        remaining -= 4
        result["behavior"] = {
            "value": hex_uint32(behavior),
            "flags": format_behavior(behavior),
        }

    if remaining >= 4:
        command_stack, next_cursor, consumed = parse_command_stack_block(data, cursor, remaining)
        if command_stack is not None:
            result["commandStack"] = command_stack
            cursor = next_cursor
            remaining -= consumed

    if remaining >= 4:
        drop_list, next_cursor, consumed = parse_drop_list(data, cursor, remaining)
        if drop_list is not None:
            result["dropList"] = drop_list
            cursor = next_cursor
            remaining -= consumed

    if remaining > 0:
        name_bytes = data[cursor:cursor + remaining]
        result["nameText"] = decode_c_string(name_bytes)
        result["nameTextBytes"] = name_bytes.hex()
        cursor += remaining

    expected_end = payload_start + sub_size
    if cursor != expected_end:
        raise ValueError(
            f"SPRD sub parse size mismatch at 0x{offset:X}: "
            f"expected 0x{expected_end:X}, got 0x{cursor:X}"
        )

    return result, cursor


def parse_sprd(data, offset):
    """解析 SPRD 精灵状态块。"""
    read_magic(data, offset, MAGIC_SPRD)
    sprd_size = read_uint32(data, offset + 4)
    options = read_uint32(data, offset + 8)
    packed_diff = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)

    cursor = offset + 20
    subs = []
    for _ in range(subresource_count):
        sub, cursor = parse_sprd_sub(data, cursor)
        subs.append(sub)

    result = {
        "SPRD_Magic": "SPRD",
        "sprdSize": sprd_size,
        "options": hex_uint32(options),
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "subs": subs,
    }
    print(f"[SPRD] subresourceCount={subresource_count}")
    return result, offset + 8 + sprd_size


def parse_sprd_padding(data, offset):
    """解析 SPRD 后用于 2 字节对齐的 0x00 padding。"""
    if offset < len(data) and offset % 2 != 0 and data[offset] == 0:
        print(f"[SPRD] alignment padding at 0x{offset:X}")
        return {"offset": offset, "value": "0x00"}, offset + 1
    return None, offset


def parse_play(data, offset):
    """解析 AS2 固定 0x30 PLAY 块。"""
    read_magic(data, offset, MAGIC_PLAY)
    play_size = read_uint32(data, offset + 4)
    options = read_uint32(data, offset + 8)
    packed_diff = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)
    play_info_size = read_uint32(data, offset + 20)
    cursor = offset + 24

    player_1 = read_uint32(data, cursor)
    player_2 = read_uint32(data, cursor + 4)
    player_3 = read_uint32(data, cursor + 8)
    player_4 = read_uint32(data, cursor + 12)
    cursor += 16

    reserved_size = play_info_size - 16
    reserved = data[cursor:cursor + reserved_size]

    result = {
        "PLAY_Magic": "PLAY",
        "playSize": play_size,
        "options": hex_uint32(options),
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "playInfoSize": play_info_size,
        "player1_ObjectId": player_1,
        "player2_ObjectId": player_2,
        "player3_ObjectId": player_3,
        "player4_ObjectId": player_4,
        "reserved": reserved.hex(),
    }
    print(f"[PLAY] players=({player_1}, {player_2}, {player_3}, {player_4})")
    return result, offset + 8 + play_size


def parse_group_data(data, offset, end_pos):
    """解析 GROU 内部成员列表。"""
    cursor = offset
    groups = []
    global_terminator = None

    while cursor < end_pos:
        value = read_uint32(data, cursor)
        if value == TERMINATOR:
            global_terminator = hex_uint32(value)
            cursor += 4
            break

        members = []
        while cursor < end_pos:
            member_value = read_uint32(data, cursor)
            cursor += 4
            if member_value == TERMINATOR:
                break
            members.append(member_value)

        groups.append({"members": members})

    return groups, global_terminator, cursor


def parse_grou(data, offset):
    """解析 GROU 分组块。"""
    read_magic(data, offset, MAGIC_GROU)
    group_size = read_uint32(data, offset + 4)
    options = read_uint32(data, offset + 8)
    packed_diff = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)
    group_info_size = read_uint32(data, offset + 20)

    groups = []
    global_terminator = None
    if group_info_size > 0:
        start = offset + 24
        end_pos = start + group_info_size
        groups, global_terminator, _ = parse_group_data(data, start, end_pos)

    result = {
        "GROU_Magic": "GROU",
        "groupSize": group_size,
        "options": hex_uint32(options),
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "groupInfoSize": group_info_size,
        "groups": groups,
    }
    if global_terminator is not None:
        result["globalTerminator"] = global_terminator

    print(f"[GROU] groupCount={len(groups)}, groupInfoSize={group_info_size}")
    return result, offset + 8 + group_size


def parse_map(map_path):
    """解析完整 AS2/ZS2 map 文件。"""
    map_path = Path(map_path)
    if not map_path.exists():
        raise FileNotFoundError(f"Map file not found: {map_path}")

    data = map_path.read_bytes()
    print(f"Parsing {map_path.name} ({len(data)} bytes)")
    print("=" * 50)

    offset = 0
    file_header, offset = parse_file_header(data, offset)
    graph, offset = parse_grph(data, offset)
    head, offset = parse_head(data, offset)
    spr, offset = parse_spr(data, offset)
    sprd, offset = parse_sprd(data, offset)
    sprd_padding, offset = parse_sprd_padding(data, offset)
    play, offset = parse_play(data, offset)
    group, offset = parse_grou(data, offset)

    print("=" * 50)
    remaining = len(data) - offset
    if remaining > 0:
        print(f"[WARN] {remaining} unparsed bytes remaining after GROU")
    else:
        print(f"Parse complete. All {len(data)} bytes consumed.")

    result = {
        "sourceFile": map_path.name,
        "totalBytes": len(data),
        "fileHeader": file_header,
        "GRPH": graph,
        "HEAD": head,
        "SPR": spr,
        "SPRD": sprd,
        "PLAY": play,
        "GROU": group,
    }
    if sprd_padding is not None:
        result["SPRD_Padding"] = sprd_padding

    return result


def main():
    """命令行入口。"""
    if len(sys.argv) < 2:
        print("用法: python as2_map_unpack.py <map_file> [--output <json_file>]")
        print("示例: python as2_map_unpack.py empty\\AS2empty.map --output empty\\AS2empty.json")
        sys.exit(1)

    map_file = sys.argv[1]
    output_file = None

    if "--output" in sys.argv:
        output_index = sys.argv.index("--output")
        if output_index + 1 < len(sys.argv):
            output_file = sys.argv[output_index + 1]

    result = parse_map(map_file)
    json_text = json.dumps(result, indent=2, ensure_ascii=False)

    if output_file:
        output_path = Path(output_file)
        output_path.write_text(json_text, encoding="utf-8")
        print(f"\nJSON saved to {output_path}")
    else:
        print("\n" + json_text)


if __name__ == "__main__":
    main()

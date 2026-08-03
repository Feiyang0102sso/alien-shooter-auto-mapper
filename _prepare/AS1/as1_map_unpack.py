"""
Alien Shooter 1 / Zombie Shooter 1 地图文件解包脚本

根据 AS1_MAP.bt 二进制模板解析 .map 文件（RES 容器格式），
将所有数据段（FileHeader、GRPH、HEAD、SPR、SPRD、PLAY、GROU）
解析为结构化 JSON 并输出。

用法:
    python as1_map_unpack.py <map_file> [--output <json_file>]

参考: AS1_MAP.bt (010 Editor Binary Template)
"""

import struct
import json
import sys
from pathlib import Path

# ============================================================
# 常量定义
# ============================================================

# 魔数 (Magic Numbers)
MAGIC_RES  = b"RES "
MAGIC_MAP  = b"MAP "
MAGIC_GRPH = b"GRPH"
MAGIC_HEAD = b"HEAD"
MAGIC_SPR  = b"SPR "
MAGIC_SPRD = b"SPRD"
MAGIC_PLAY = b"PLAY"
MAGIC_GROU = b"GROU"

# 每条 SpriteRecord 的大小: 7 * 4 = 28 字节
SPRITE_RECORD_SIZE = 28

# SingleCommandRecord: 4 * uint32 = 16 字节
SINGLE_COMMAND_SIZE = 16

# SPRD_Sub 终止标记
SPRD_TERMINATOR = 0xFFFFFFFF

# 动作 ID 名称映射表
COMMAND_ID_NAMES = {
    # ANI_* 动画类
    0:  "ANI_STAND",
    1:  "ANI_STOP_MOVE_ANI_BUILD",
    2:  "ANI_GO",
    3:  "ANI_START_MOVE",
    4:  "ANI_L_ROTATE",
    5:  "ANI_R_ROTATE",
    6:  "ANI_OPEN",
    7:  "ANI_HIT",
    8:  "ANI_FIGHT",
    9:  "ANI_SALUT",
    10: "ANI_STAND_OPEN",
    11: "ANI_CLASH_VERT",
    12: "ANI_LAZY_ANI_CLASH",
    13: "ANI_WOUND",
    14: "ANI_BIRTH",
    15: "ANI_DEATH",
    16: "ANI_DEATH2",
    # ACT_* 行为类
    32:  "ACT_ATTACK",
    33:  "ACT_MOVE",
    34:  "ACT_MOVE_TO",
    35:  "ACT_BUILD_UNIT",
    36:  "ACT_PATROL",
    37:  "ACT_COOR_ATTACK",
    38:  "ACT_RANDOM",
    39:  "ACT_STOP",
    40:  "ACT_PAUSE",
    41:  "ACT_ROTATE",
    43:  "ACT_FLAGMAN_TRIGGER",
    62:  "ACT_CHANGE_VID",
    63:  "ACT_CHANGE_COOR",
    71:  "ACT_GOTO_STACK",
    72:  "ACT_CLEAR_STACK",
    73:  "ACT_STOP_STACK",
    79:  "ACT_WHILE_NOT_SCRIPT_VAR",
    85:  "ACT_DAMAGE",
    88:  "ACT_SET_HP",
    91:  "ACT_SET_GOAL_COOR",
    95:  "ACT_SET_BEHAVE",
    97:  "ACT_SET_ARMY",
    98:  "ACT_SET_INVISIBLE",
    102: "ACT_SET_LINK",
    104: "ACT_SET_UPLINK",
    131: "ACT_SCRIPT_VAR",
    134: "ACT_DESTROY_UNIT",
    135: "ACT_PLAY_SFX",
}


def get_command_name(action_id):
    """根据动作 ID 返回可读名称。"""
    name = COMMAND_ID_NAMES.get(action_id)
    if name:
        return f"{name} ({action_id})"
    return f"UNKNOWN_COMMAND ({action_id})"


def get_behavior_flags(value):
    """
    解析行为标志位。

    bit 0: Aggressive
    bit 1: Active
    bit 2: Careful
    """
    if value > 7:
        return f"ERROR: Behavior > 0x07 (value={value})"

    flags = []
    if value & 1:
        flags.append("Aggressive")
    if value & 2:
        flags.append("Active")
    if value & 4:
        flags.append("Careful")

    if not flags:
        return "None"
    return " ".join(flags)


# ============================================================
# 读取辅助函数
# ============================================================

def read_uint32(data, offset):
    """读取一个小端 uint32。"""
    return struct.unpack_from("<I", data, offset)[0]


def read_int32(data, offset):
    """读取一个小端 int32。"""
    return struct.unpack_from("<i", data, offset)[0]


def read_float(data, offset):
    """读取一个小端 float。"""
    return struct.unpack_from("<f", data, offset)[0]


def read_magic(data, offset, expected):
    """
    读取并校验 4 字节魔数。

    如果不匹配则抛出 ValueError。
    """
    actual = data[offset:offset + 4]
    if actual != expected:
        raise ValueError(
            f"Magic mismatch at offset 0x{offset:X}: "
            f"expected {expected!r}, got {actual!r}"
        )
    return actual.decode("ascii")


# ============================================================
# 各数据段解析函数
# ============================================================

def parse_file_header(data, offset):
    """
    解析 FileHeader (12 字节)。

    结构:
        char[4] RES_Magic   - "RES "
        uint32  mapSize     - 地图总大小
        char[4] MAP_Magic   - "MAP "
    """
    read_magic(data, offset, MAGIC_RES)
    map_size = read_uint32(data, offset + 4)
    read_magic(data, offset + 8, MAGIC_MAP)

    result = {
        "RES_Magic": "RES ",
        "mapSize": map_size,
        "MAP_Magic": "MAP ",
    }
    print(f"[FileHeader] mapSize={map_size} (0x{map_size:X})")
    return result, offset + 12


def parse_grph(data, offset):
    """
    解析 GRPH_Data (0x2C 字节)。

    包含图形环境参数: 阴影、伽马、风向、风速等。
    """
    read_magic(data, offset, MAGIC_GRPH)
    graph_size        = read_uint32(data, offset + 4)
    options           = read_uint32(data, offset + 8)
    packed_diff       = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)
    env_size          = read_uint32(data, offset + 20)
    is_shadow         = read_uint32(data, offset + 24)
    gamma_darker      = read_uint32(data, offset + 28)
    gamma_lighter     = read_uint32(data, offset + 32)
    wind_direction    = read_uint32(data, offset + 36)
    wind_speed        = read_float(data, offset + 40)

    result = {
        "GRPH_Magic": "GRPH",
        "graphSize": graph_size,
        "options": f"0x{options:08X}",
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "envSize": env_size,
        "isShadow": is_shadow,
        "gammaDarker": f"0x{gamma_darker:06X}",
        "gammaLighter": f"0x{gamma_lighter:06X}",
        "windDirection": wind_direction,
        "windSpeed": wind_speed,
    }
    # 总长度: 4(magic) + 4(size) + graphSize
    print(f"[GRPH] graphSize={graph_size}, shadow={is_shadow}, wind={wind_direction}/{wind_speed}")
    return result, offset + 8 + graph_size


def parse_head(data, offset):
    """
    解析 HEAD_Data (0x30 字节)。

    包含地图尺寸、摄像机默认位置、版本号等核心信息。
    """
    read_magic(data, offset, MAGIC_HEAD)
    head_size         = read_uint32(data, offset + 4)
    options           = read_uint32(data, offset + 8)
    packed_diff       = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)
    map_info_size     = read_uint32(data, offset + 20)
    map_size_x        = read_float(data, offset + 24)
    map_size_y        = read_float(data, offset + 28)
    map_camera_x      = read_float(data, offset + 32)
    map_camera_y      = read_float(data, offset + 36)
    current_time      = read_uint32(data, offset + 40)
    version           = read_int32(data, offset + 44)

    result = {
        "HEAD_Magic": "HEAD",
        "headSize": head_size,
        "options": f"0x{options:08X}",
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "mapInfoSize": map_info_size,
        "mapSizeX": map_size_x,
        "mapSizeY": map_size_y,
        "mapCameraX": map_camera_x,
        "mapCameraY": map_camera_y,
        "currentTime": current_time,
        "version": f"0x{version:02X}",
    }
    print(
        f"[HEAD] mapSize=({map_size_x}, {map_size_y}), "
        f"camera=({map_camera_x}, {map_camera_y}), version=0x{version:02X}"
    )
    return result, offset + 8 + head_size


def parse_spr(data, offset):
    """
    解析 SPR_Data (可变长度)。

    包含所有精灵/物体的位置和基础属性。
    每条 SpriteRecord 为 28 字节 (7 * uint32/float)。
    """
    read_magic(data, offset, MAGIC_SPR)
    spr_size          = read_uint32(data, offset + 4)
    options           = read_uint32(data, offset + 8)
    packed_diff       = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)
    spr_info_size     = read_uint32(data, offset + 20)

    # 精灵数量: (sprInfoSize - 4) / 28
    # 末尾还有 4 字节 terminator
    sprite_count = (spr_info_size - 4) // SPRITE_RECORD_SIZE

    sprites = []
    sprite_offset = offset + 24
    for i in range(sprite_count):
        base = sprite_offset + i * SPRITE_RECORD_SIZE
        sprite = {
            "objectId":  read_uint32(data, base),
            "vid":       read_int32(data, base + 4),
            "posX":      read_float(data, base + 8),
            "posY":      read_float(data, base + 12),
            "posZ":      read_float(data, base + 16),
            "direction": read_uint32(data, base + 20),
            "army":      read_int32(data, base + 24),
        }
        sprites.append(sprite)

    terminator_offset = sprite_offset + sprite_count * SPRITE_RECORD_SIZE
    terminator = read_uint32(data, terminator_offset)

    result = {
        "SPR_Magic": "SPR ",
        "sprSize": spr_size,
        "options": f"0x{options:08X}",
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "sprInfoSize": spr_info_size,
        "spriteCount": sprite_count,
        "sprites": sprites,
        "terminator": f"0x{terminator:08X}",
    }
    print(f"[SPR] spriteCount={sprite_count}, sprSize={spr_size}")
    return result, offset + 8 + spr_size


def parse_sprd_sub(data, offset):
    """
    解析单个 SPRD_Sub (可变长度)。

    每个 sub 包含:
        - subSize: 数据总长度
        - objectId: 关联的物体ID
        - CommandStack: 指令栈
        - DropList: 掉落物列表
        - behavior: 行为标志 (可选)

    如果 subSize == 4，表示终止标记。
    """
    sub_size = read_uint32(data, offset)
    cursor = offset + 4

    # 终止子段: subSize == 4，后跟 4 字节终止符
    if sub_size == 4:
        terminator = read_uint32(data, cursor)
        result = {
            "subSize": sub_size,
            "isTerminator": True,
            "terminator": f"0x{terminator:08X}",
        }
        return result, cursor + 4

    # 正常子段
    payload_start = cursor
    object_id = read_uint32(data, cursor)
    cursor += 4

    # 解析 CommandStack
    cmd_count = read_uint32(data, cursor)
    cursor += 4
    commands = []
    for _ in range(cmd_count):
        cmd = {
            "actionId": read_uint32(data, cursor),
            "actionName": get_command_name(read_uint32(data, cursor)),
            "param1": read_uint32(data, cursor + 4),
            "param2": read_uint32(data, cursor + 8),
            "param3": read_uint32(data, cursor + 12),
        }
        commands.append(cmd)
        cursor += SINGLE_COMMAND_SIZE

    # 解析 DropList
    item_count = read_uint32(data, cursor)
    cursor += 4
    drop_items = []
    for _ in range(item_count):
        drop_items.append(read_uint32(data, cursor))
        cursor += 4

    # 计算已消费字节数和剩余量
    consumed = cursor - payload_start
    remaining = sub_size - consumed

    behavior = None
    unknown_bytes = None

    if remaining == 4:
        behavior_value = read_uint32(data, cursor)
        behavior = {
            "value": behavior_value,
            "flags": get_behavior_flags(behavior_value),
        }
        cursor += 4
    elif remaining > 0:
        print(f"  [WARN] Unexpected {remaining} bytes remaining in SPRD_Sub at 0x{cursor:X}")
        unknown_bytes = data[cursor:cursor + remaining].hex()
        cursor += remaining

    result = {
        "subSize": sub_size,
        "isTerminator": False,
        "objectId": object_id,
        "commandStack": {
            "cmdCount": cmd_count,
            "commands": commands,
        },
        "dropList": {
            "itemCount": item_count,
            "itemVids": drop_items,
        },
    }
    if behavior is not None:
        result["behavior"] = behavior
    if unknown_bytes is not None:
        result["unknownBytes"] = unknown_bytes

    return result, cursor


def parse_sprd(data, offset):
    """
    解析 SPRD_Data (可变长度)。

    包含所有精灵的详细数据（指令、掉落物、行为）。
    子段数量 = subresourceCount（含终止子段）。
    """
    read_magic(data, offset, MAGIC_SPRD)
    sprd_size         = read_uint32(data, offset + 4)
    options           = read_uint32(data, offset + 8)
    packed_diff       = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)

    cursor = offset + 20
    subs = []
    for _ in range(subresource_count):
        sub, cursor = parse_sprd_sub(data, cursor)
        subs.append(sub)

    result = {
        "SPRD_Magic": "SPRD",
        "sprdSize": sprd_size,
        "options": f"0x{options:08X}",
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "subs": subs,
    }
    print(f"[SPRD] subresourceCount={subresource_count}, sprdSize={sprd_size}")
    return result, offset + 8 + sprd_size


def parse_play(data, offset):
    """
    解析 PLAY_Data (0x28 字节)。

    包含 4 个玩家的 ObjectId 映射。
    """
    read_magic(data, offset, MAGIC_PLAY)
    play_size         = read_uint32(data, offset + 4)
    options           = read_uint32(data, offset + 8)
    packed_diff       = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)
    play_info_size    = read_uint32(data, offset + 20)
    p1_id             = read_uint32(data, offset + 24)
    p2_id             = read_uint32(data, offset + 28)
    p3_id             = read_uint32(data, offset + 32)
    p4_id             = read_uint32(data, offset + 36)

    result = {
        "PLAY_Magic": "PLAY",
        "playSize": play_size,
        "options": f"0x{options:08X}",
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "playInfoSize": play_info_size,
        "player1_ObjectId": p1_id,
        "player2_ObjectId": p2_id,
        "player3_ObjectId": p3_id,
        "player4_ObjectId": p4_id,
    }
    print(f"[PLAY] players=({p1_id}, {p2_id}, {p3_id}, {p4_id})")
    return result, offset + 8 + play_size


def parse_grou(data, offset):
    """
    解析 GROU_Data (可变长度)。

    包含物体分组信息。每组以 0xFFFFFFFF 结尾，
    所有组以全局 0xFFFFFFFF 结尾。
    """
    read_magic(data, offset, MAGIC_GROU)
    group_size        = read_uint32(data, offset + 4)
    options           = read_uint32(data, offset + 8)
    packed_diff       = read_uint32(data, offset + 12)
    subresource_count = read_uint32(data, offset + 16)
    group_info_size   = read_uint32(data, offset + 20)

    groups = []
    global_terminator = None

    if group_info_size > 0:
        cursor = offset + 24
        end_pos = cursor + group_info_size

        while cursor < end_pos:
            # 检查是否遇到全局终止符（最后一个 0xFFFFFFFF 且后面不再有数据）
            val = read_uint32(data, cursor)
            if val == SPRD_TERMINATOR:
                # 如果剩余刚好 4 字节，这是全局终止符
                if cursor + 4 >= end_pos:
                    global_terminator = f"0x{val:08X}"
                    cursor += 4
                    break

            # 读取一个 group: 成员列表 + 0xFFFFFFFF 终止
            members = []
            while cursor < end_pos:
                member_val = read_uint32(data, cursor)
                cursor += 4
                if member_val == SPRD_TERMINATOR:
                    break
                members.append(member_val)

            groups.append({
                "members": members,
            })

    result = {
        "GROU_Magic": "GROU",
        "groupSize": group_size,
        "options": f"0x{options:08X}",
        "packedDiff": packed_diff,
        "subresourceCount": subresource_count,
        "groupInfoSize": group_info_size,
        "groups": groups,
    }
    if global_terminator is not None:
        result["globalTerminator"] = global_terminator

    print(f"[GROU] groupCount={len(groups)}, groupInfoSize={group_info_size}")
    return result, offset + 8 + group_size


# ============================================================
# 主解析入口
# ============================================================

def parse_map(map_path):
    """
    解析整个 .map 文件。

    按顺序读取六个数据段:
        FileHeader → GRPH → HEAD → SPR → SPRD → PLAY → GROU

    返回包含所有段数据的字典。
    """
    map_path = Path(map_path)
    if not map_path.exists():
        raise FileNotFoundError(f"Map file not found: {map_path}")

    data = map_path.read_bytes()
    print(f"Parsing {map_path.name} ({len(data)} bytes)")
    print("=" * 50)

    offset = 0

    file_header, offset = parse_file_header(data, offset)
    grph, offset        = parse_grph(data, offset)
    head, offset        = parse_head(data, offset)
    spr, offset         = parse_spr(data, offset)
    sprd, offset        = parse_sprd(data, offset)
    play, offset        = parse_play(data, offset)
    grou, offset        = parse_grou(data, offset)

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
        "GRPH": grph,
        "HEAD": head,
        "SPR": spr,
        "SPRD": sprd,
        "PLAY": play,
        "GROU": grou,
    }
    return result


def main():
    """命令行入口。"""
    if len(sys.argv) < 2:
        print("用法: python as1_map_unpack.py <map_file> [--output <json_file>]")
        print("示例: python as1_map_unpack.py new.map --output new.json")
        sys.exit(1)

    map_file = sys.argv[1]

    # 解析 --output 参数
    output_file = None
    if "--output" in sys.argv:
        idx = sys.argv.index("--output")
        if idx + 1 < len(sys.argv):
            output_file = sys.argv[idx + 1]

    result = parse_map(map_file)

    # 输出 JSON
    json_str = json.dumps(result, indent=2, ensure_ascii=False)

    if output_file:
        output_path = Path(output_file)
        output_path.write_text(json_str, encoding="utf-8")
        print(f"\nJSON saved to {output_path}")
    else:
        print("\n" + json_str)


if __name__ == "__main__":
    main()

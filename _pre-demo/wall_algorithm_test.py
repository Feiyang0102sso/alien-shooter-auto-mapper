"""
Automapper 通用铺墙算法测试 (互斥立柱版)

验证任意线段列表转换为合法的精灵列表。
立柱与直墙互斥：若某格点是端点或交点，则仅放立柱，不放直墙。
"""

import struct
import random
from collections import defaultdict
from pathlib import Path

# ============================================================
# 常量
# ============================================================

MAP_SIZE_X = 600.0
MAP_SIZE_Y = 600.0
STEP_X = 40.0
STEP_Y = 28.0

# 走向 A (/)
VID_WALL_A_LIST = [601]
# 走向 B (\)
VID_WALL_B_LIST = [602]
VID_PILLAR = 604

DIRECTION = 32
ARMY = 0
OPTIONS = 0x80000000
TERMINATOR = 0xFFFFFFFF
MAP_VERSION = 0x0C

EMPTY_MAP_PATH = Path(__file__).parent / "empty.map"
OUTPUT_DIR = Path(__file__).parent

# ============================================================
# 基础坐标与打包工具
# ============================================================

def to_iso(gx, gy, shift_x, shift_y):
    pos_x = (gx - gy) * STEP_X + shift_x
    pos_y = (gx + gy) * STEP_Y + shift_y
    return pos_x, pos_y

def pack_uint32(value): return struct.pack("<I", value)
def pack_int32(value): return struct.pack("<i", value)
def pack_float(value): return struct.pack("<f", value)

def make_sprite(vid, gx, gy, shift_x, shift_y):
    pos_x, pos_y = to_iso(gx, gy, shift_x, shift_y)
    if vid in VID_WALL_B_LIST:
        pos_x -= 40.0
    return {
        "vid": vid,
        "posX": pos_x,
        "posY": pos_y,
        "posZ": 0.0,
        "direction": DIRECTION,
        "army": ARMY,
    }

# ============================================================
# 核心算法
# ============================================================

def build_walls_sprites(segments, map_size_x=MAP_SIZE_X, map_size_y=MAP_SIZE_Y):
    """
    输入: segments = [((x1, y1), (x2, y2)), ...]
    输出: sprite_list
    """
    edges_a = set() # (x, y) 表示从 (x, y-1) 到 (x, y) 的墙壁
    edges_b = set() # (x, y) 表示从 (x-1, y) 到 (x, y) 的墙壁

    # 1. 线段光栅化为 Edge
    for (x1, y1), (x2, y2) in segments:
        if x1 == x2: # 沿 Y 轴，走向 A (/)
            ymin, ymax = min(y1, y2), max(y1, y2)
            for y in range(ymin + 1, ymax + 1):
                edges_a.add((x1, y))
        elif y1 == y2: # 沿 X 轴，走向 B (\)
            xmin, xmax = min(x1, x2), max(x1, x2)
            for x in range(xmin + 1, xmax + 1):
                edges_b.add((x, y1))
        else:
            print(f"Warning: Ignoring non-orthogonal segment {(x1,y1)}-{(x2,y2)}")

    raw_sprites = []
    
    # 2. 将 Edge 转换为直墙精灵
    for (x, y) in edges_a:
        vid = random.choice(VID_WALL_A_LIST)
        raw_sprites.append((vid, x, y))
        
    for (x, y) in edges_b:
        vid = random.choice(VID_WALL_B_LIST)
        raw_sprites.append((vid, x, y))

    # 3. 收集所有相关的顶点 (Vertex)
    vertices = set()
    for (x, y) in edges_a:
        vertices.add((x, y-1))
        vertices.add((x, y))
    for (x, y) in edges_b:
        vertices.add((x-1, y))
        vertices.add((x, y))

    # 4. 根据顶点的连接度判定是否需要放置立柱
    for (x, y) in vertices:
        # 检查四个方向的边是否存在
        up = (x, y) in edges_a
        down = (x, y+1) in edges_a
        left = (x, y) in edges_b
        right = (x+1, y) in edges_b

        conn_a = up or down
        conn_b = left or right
        total_conns = sum([up, down, left, right])

        needs_pillar = False
        if total_conns == 1:
            needs_pillar = True # 端点
        elif conn_a and conn_b:
            needs_pillar = True # 转角、T字、十字交汇

        if needs_pillar:
            raw_sprites.append((VID_PILLAR, x, y))

    if not raw_sprites:
        return []

    # 5. 为了防止 MapEdit / 引擎因为渲染顺序不对而导致的丢块、遮挡问题
    # 我们必须进行 Z-Order 排序 (Painter's algorithm): 从后往前画
    # 在等轴测坐标系中，y 值越小越在屏幕上方（后方）。
    # pos_y 与 (gx + gy) 成正比，因此根据 gx + gy 升序排序
    raw_sprites.sort(key=lambda s: s[1] + s[2])

    # 6. 居中偏置计算
    all_x = []
    all_y = []
    for vid, gx, gy in raw_sprites:
        pos_x, pos_y = to_iso(gx, gy, 0.0, 0.0)
        if vid in VID_WALL_B_LIST:
            pos_x -= 40.0
        all_x.append(pos_x)
        all_y.append(pos_y)

    min_x, max_x = min(all_x), max(all_x)
    min_y, max_y = min(all_y), max(all_y)
    width = max_x - min_x
    height = max_y - min_y

    raw_shift_x = (map_size_x - width) / 2.0 - min_x
    raw_shift_y = (map_size_y - height) / 2.0 - min_y

    grid_x = round((raw_shift_x - 20.0) / 40.0)
    shift_x = grid_x * 40.0 + 20.0
    grid_y = round((raw_shift_y - 14.0) / 28.0)
    shift_y = grid_y * 28.0 + 14.0

    # 7. 生成最终精灵
    sprites = []
    for vid, x, y in raw_sprites:
        sprites.append(make_sprite(vid, x, y, shift_x, shift_y))

    return sprites


# ============================================================
# 二进制写入器
# ============================================================

def build_spr_section(sprites):
    records = b""
    for i, spr in enumerate(sprites):
        object_id = i + 1
        records += pack_uint32(object_id)
        records += pack_int32(spr["vid"])
        records += pack_float(spr["posX"])
        records += pack_float(spr["posY"])
        records += pack_float(spr["posZ"])
        records += pack_uint32(spr["direction"])
        records += pack_int32(spr["army"])

    spr_info_size = len(records) + 4

    header = pack_uint32(OPTIONS)
    header += pack_uint32(0)
    header += pack_uint32(1)
    header += pack_uint32(spr_info_size)

    body = records + pack_uint32(TERMINATOR)
    spr_size = len(header) + len(body)

    section = b"SPR "
    section += pack_uint32(spr_size)
    section += header
    section += body
    return section

def build_sprd_section(sprites):
    subs = b""
    for i in range(len(sprites)):
        object_id = i + 1
        subs += pack_uint32(12)
        subs += pack_uint32(object_id)
        subs += pack_uint32(0)
        subs += pack_uint32(0)

    subs += pack_uint32(4)
    subs += pack_uint32(TERMINATOR)

    sub_count = len(sprites) + 1

    header = pack_uint32(OPTIONS)
    header += pack_uint32(0)
    header += pack_uint32(sub_count)

    sprd_size = len(header) + len(subs)

    section = b"SPRD"
    section += pack_uint32(sprd_size)
    section += header
    section += subs
    return section

def build_play_section():
    header = pack_uint32(OPTIONS)
    header += pack_uint32(0)
    header += pack_uint32(1)
    header += pack_uint32(16)

    body = pack_uint32(0) * 4
    play_size = len(header) + len(body)

    section = b"PLAY"
    section += pack_uint32(play_size)
    section += header
    section += body
    return section

def build_grou_section():
    header = pack_uint32(OPTIONS)
    header += pack_uint32(0)
    header += pack_uint32(1)
    header += pack_uint32(4)

    body = pack_uint32(TERMINATOR)
    grou_size = len(header) + len(body)

    section = b"GROU"
    section += pack_uint32(grou_size)
    section += header
    section += body
    return section

def write_map(sprites, output_path, map_size_x=MAP_SIZE_X, map_size_y=MAP_SIZE_Y):
    template = EMPTY_MAP_PATH.read_bytes()
    grph_section = template[12:56]

    head_header = pack_uint32(OPTIONS)
    head_header += pack_uint32(0)
    head_header += pack_uint32(1)
    head_header += pack_uint32(24)

    head_body = pack_float(map_size_x)
    head_body += pack_float(map_size_y)
    head_body += pack_float(0.0)
    head_body += pack_float(0.0)
    head_body += pack_uint32(0)
    head_body += pack_int32(MAP_VERSION)

    head_size = len(head_header) + len(head_body)
    head_section = b"HEAD"
    head_section += pack_uint32(head_size)
    head_section += head_header
    head_section += head_body

    spr_section = build_spr_section(sprites)
    sprd_section = build_sprd_section(sprites)
    play_section = build_play_section()
    grou_section = build_grou_section()

    payload = grph_section + head_section + spr_section + sprd_section + play_section + grou_section
    map_size = 4 + len(payload)

    file_header = b"RES "
    file_header += pack_uint32(map_size)
    file_header += b"MAP "

    full_data = file_header + payload
    output_path = Path(output_path)
    output_path.write_bytes(full_data)

    print(f"[{output_path.name}] 生成完毕！共 {len(sprites)} 个精灵。")

# ============================================================
# 测试场景
# ============================================================

def test_scene1_l_shape():
    """L型房间：单边转角测试"""
    segments = [
        ((0, 0), (5, 0)), # 上边
        ((0, 0), (0, 5)), # 左上边
        ((0, 5), (3, 5)), # 底部左段
        ((3, 5), (3, 2)), # L内凹转角
        ((3, 2), (5, 2)), # 底部右段
        ((5, 2), (5, 0)), # 右边
    ]
    sprites = build_walls_sprites(segments)
    write_map(sprites, OUTPUT_DIR / "test_scene1_L_shape.map")

def test_scene2_t_junction():
    """T字形内部隔断：测试T型立柱生成"""
    segments = [
        # 外框 (6x6)
        ((0, 0), (6, 0)),
        ((0, 0), (0, 6)),
        ((0, 6), (6, 6)),
        ((6, 0), (6, 6)),
        # 内部隔断 (T字)
        ((3, 0), (3, 3)), # 从上边垂直向下
        ((0, 3), (3, 3)), # 从左边水平向右，与上边的隔断在 (3,3) 相交
    ]
    sprites = build_walls_sprites(segments)
    write_map(sprites, OUTPUT_DIR / "test_scene2_T_junction.map")

def test_scene3_cross():
    """十字交叉：测试十字交汇点的包覆"""
    segments = [
        ((2, 0), (2, 4)), # 竖向长廊
        ((0, 2), (4, 2)), # 横向长廊
    ]
    sprites = build_walls_sprites(segments)
    write_map(sprites, OUTPUT_DIR / "test_scene3_cross.map")

if __name__ == "__main__":
    test_scene1_l_shape()
    test_scene2_t_junction()
    test_scene3_cross()

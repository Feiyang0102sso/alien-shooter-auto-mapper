"""
Automapper Demo: 墙壁铺设公式验证

最简验证脚本：
  - Test 1: 铺一排走向 A 墙壁（/），验证投影公式
  - Test 2: 铺一排走向 B 墙壁（\），验证偏置 -40
  - Test 3: 用 4 根立柱 + 4 面墙拼一个 5×5 方形房间

生成的 .map 文件直接用 MapEdit.exe 打开验证。
"""

import struct
from pathlib import Path

# ============================================================
# 常量
# ============================================================

# 地图尺寸
MAP_SIZE_X = 600.0
MAP_SIZE_Y = 600.0

# 投影公式参数
STEP_X = 40.0
STEP_Y = 28.0

# 图块 VID
# 走向 A (VID 601): 贴图朝向 /，沿 gy 轴铺设（gx 固定）
# 走向 B (VID 602): 贴图朝向 \，沿 gx 轴铺设（gy 固定），posX 额外 -40
VID_WALL_A = 601
VID_WALL_B = 602
VID_PILLAR = 604

# 固定参数
DIRECTION = 32
ARMY = 0

# 二进制格式常量
OPTIONS = 0x80000000
TERMINATOR = 0xFFFFFFFF
MAP_VERSION = 0x0C

# empty.map 模板路径
EMPTY_MAP_PATH = Path(__file__).parent / "empty.map"

# 输出目录
OUTPUT_DIR = Path(__file__).parent


# ============================================================
# 投影公式
# ============================================================

def to_iso(gx, gy, shift_x, shift_y):
    """
    逻辑格点 (gx, gy) → 物理坐标 (posX, posY)。

    posX = (gx - gy) * 40.0 + shiftX
    posY = (gx + gy) * 28.0 + shiftY
    """
    pos_x = (gx - gy) * STEP_X + shift_x
    pos_y = (gx + gy) * STEP_Y + shift_y
    return pos_x, pos_y


# ============================================================
# 精灵生成
# ============================================================

def make_sprite(vid, gx, gy, shift_x, shift_y):
    """
    根据 VID 和逻辑坐标生成一个精灵字典。

    走向 B 墙壁 (VID 602) 需要额外 posX -= 40.0 的偏置。
    立柱和走向 A 墙壁无偏置。
    """
    pos_x, pos_y = to_iso(gx, gy, shift_x, shift_y)

    # 走向 B 偏置修正
    if vid in (VID_WALL_B, 603, 612):
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
# 测试用例
# ============================================================

def test1_wall_a(shift_x, shift_y):
    """
    Test 1: 一排走向 A 墙壁 (/)。

    VID 601 沿 gy 轴铺设（gx 固定）。
    gx=0, gy=0..4
    """
    sprites = []
    gx = 0
    for gy in range(5):
        sprites.append(make_sprite(VID_WALL_A, gx, gy, shift_x, shift_y))
    return sprites


def test2_wall_b(shift_x, shift_y):
    """
    Test 2: 一排走向 B 墙壁 (\\)。

    VID 602 沿 gx 轴铺设（gy 固定），posX 额外 -40 偏置。
    gx=0..4, gy=0
    """
    sprites = []
    gy = 0
    for gx in range(5):
        sprites.append(make_sprite(VID_WALL_B, gx, gy, shift_x, shift_y))
    return sprites


def test3_square_room(shift_x, shift_y):
    """
    Test 3: 5×5 方形房间，复刻 room_sample.json 的结构。

    四角立柱: (0,0), (5,0), (5,5), (0,5)

    走向 A (VID 601, /): 沿 gy 轴铺设（gx 固定）
      左边: gx=0, gy=1..4
      右边: gx=5, gy=1..4

    走向 B (VID 602, \\): 沿 gx 轴铺设（gy 固定）
      上边: gy=0, gx=1..4
      下边: gy=5, gx=1..4
    """
    sprites = []

    # 四角立柱
    for gx, gy in [(0, 0), (5, 0), (5, 5), (0, 5)]:
        sprites.append(make_sprite(VID_PILLAR, gx, gy, shift_x, shift_y))

    # 走向 A (VID 601, /): 沿 gy 轴（gx 固定）
    # 左边: gx=0, gy=1..5（gy=5 与角落立柱叠放）
    for gy in range(1, 6):
        sprites.append(make_sprite(VID_WALL_A, 0, gy, shift_x, shift_y))
    # 右边: gx=5, gy=1..5
    for gy in range(1, 6):
        sprites.append(make_sprite(VID_WALL_A, 5, gy, shift_x, shift_y))

    # 走向 B (VID 602, \\): 沿 gx 轴（gy 固定）
    # 上边: gy=0, gx=1..5（gx=5 与角落立柱叠放）
    for gx in range(1, 6):
        sprites.append(make_sprite(VID_WALL_B, gx, 0, shift_x, shift_y))
    # 下边: gy=5, gx=1..5
    for gx in range(1, 6):
        sprites.append(make_sprite(VID_WALL_B, gx, 5, shift_x, shift_y))

    return sprites


# ============================================================
# 二进制 .map 写入器
# ============================================================

def pack_uint32(value):
    """打包一个 Little-Endian uint32。"""
    return struct.pack("<I", value)


def pack_int32(value):
    """打包一个 Little-Endian int32。"""
    return struct.pack("<i", value)


def pack_float(value):
    """打包一个 Little-Endian float。"""
    return struct.pack("<f", value)


def build_spr_section(sprites):
    """
    构建 SPR 段的完整二进制数据。

    结构:
        "SPR " + sprSize + options + packedDiff + subCount + sprInfoSize
        + N × SpriteRecord (28B each)
        + terminator (0xFFFFFFFF)
    """
    # 逐条打包 SpriteRecord
    records = b""
    for i, spr in enumerate(sprites):
        object_id = i + 1  # 从 1 开始自增
        records += pack_uint32(object_id)
        records += pack_int32(spr["vid"])
        records += pack_float(spr["posX"])
        records += pack_float(spr["posY"])
        records += pack_float(spr["posZ"])
        records += pack_uint32(spr["direction"])
        records += pack_int32(spr["army"])

    # sprInfoSize = 精灵数据 + terminator (4B)
    spr_info_size = len(records) + 4

    # 段头: options + packedDiff + subCount + sprInfoSize = 16B
    header = pack_uint32(OPTIONS)          # options
    header += pack_uint32(0)               # packedDiff
    header += pack_uint32(1)               # subresourceCount
    header += pack_uint32(spr_info_size)   # sprInfoSize

    # 段体: 精灵记录 + terminator
    body = records + pack_uint32(TERMINATOR)

    # sprSize = header + body 的总长度
    spr_size = len(header) + len(body)

    # 完整段: magic + size + 内容
    section = b"SPR "
    section += pack_uint32(spr_size)
    section += header
    section += body

    return section


def build_sprd_section(sprites):
    """
    构建 SPRD 段的完整二进制数据。

    墙壁精灵的 SPRD sub 全部为固定模板：
        subSize(12) + objectId + cmdCount(0) + itemCount(0)

    末尾加一个 terminator sub:
        subSize(4) + 0xFFFFFFFF
    """
    subs = b""
    for i in range(len(sprites)):
        object_id = i + 1
        subs += pack_uint32(12)            # subSize: payload 为 12 字节
        subs += pack_uint32(object_id)     # objectId
        subs += pack_uint32(0)             # cmdCount
        subs += pack_uint32(0)             # itemCount

    # terminator sub
    subs += pack_uint32(4)                 # subSize: payload 为 4 字节
    subs += pack_uint32(TERMINATOR)        # terminator

    # subresourceCount = 精灵数 + 1 (terminator sub)
    sub_count = len(sprites) + 1

    # 段头: options + packedDiff + subresourceCount = 12B
    header = pack_uint32(OPTIONS)
    header += pack_uint32(0)
    header += pack_uint32(sub_count)

    # sprdSize = header + subs
    sprd_size = len(header) + len(subs)

    section = b"SPRD"
    section += pack_uint32(sprd_size)
    section += header
    section += subs

    return section


def build_play_section():
    """
    构建 PLAY 段（固定 40B）。

    4 个玩家 ObjectId 全部为 0。
    """
    header = pack_uint32(OPTIONS)
    header += pack_uint32(0)               # packedDiff
    header += pack_uint32(1)               # subresourceCount
    header += pack_uint32(16)              # playInfoSize

    # 4 个玩家 ID
    body = pack_uint32(0) * 4

    play_size = len(header) + len(body)

    section = b"PLAY"
    section += pack_uint32(play_size)
    section += header
    section += body

    return section


def build_grou_section():
    """
    构建空 GROU 段。

    无分组，只有一个全局 terminator。
    """
    header = pack_uint32(OPTIONS)
    header += pack_uint32(0)               # packedDiff
    header += pack_uint32(1)               # subresourceCount
    header += pack_uint32(4)               # groupInfoSize: 只有 terminator

    body = pack_uint32(TERMINATOR)

    grou_size = len(header) + len(body)

    section = b"GROU"
    section += pack_uint32(grou_size)
    section += header
    section += body

    return section


def write_map(sprites, output_path, map_size_x=MAP_SIZE_X, map_size_y=MAP_SIZE_Y):
    """
    将精灵列表写为完整的 .map 二进制文件。

    流程:
        1. 从 empty.map 读取 GRPH 段原样复用
        2. 重建 HEAD 段（写入 map_size_x/y）
        3. 构建 SPR / SPRD / PLAY / GROU 段
        4. 拼接所有段
        5. 回填 FileHeader 的 mapSize
    """
    # 读取 empty.map 模板
    template = EMPTY_MAP_PATH.read_bytes()

    # ---- GRPH 段: 从 empty.map 偏移 12 开始, 长度 = 4(magic) + 4(size) + 36(data) = 44B ----
    grph_section = template[12:56]

    # ---- HEAD 段: 重建 ----
    head_header = pack_uint32(OPTIONS)
    head_header += pack_uint32(0)          # packedDiff
    head_header += pack_uint32(1)          # subresourceCount
    head_header += pack_uint32(24)         # mapInfoSize

    head_body = pack_float(map_size_x)     # mapSizeX
    head_body += pack_float(map_size_y)    # mapSizeY
    head_body += pack_float(0.0)           # mapCameraX (默认原点)
    head_body += pack_float(0.0)           # mapCameraY
    head_body += pack_uint32(0)            # currentTime
    head_body += pack_int32(MAP_VERSION)   # version: 0x0C

    head_size = len(head_header) + len(head_body)

    head_section = b"HEAD"
    head_section += pack_uint32(head_size)
    head_section += head_header
    head_section += head_body

    # ---- SPR / SPRD / PLAY / GROU ----
    spr_section = build_spr_section(sprites)
    sprd_section = build_sprd_section(sprites)
    play_section = build_play_section()
    grou_section = build_grou_section()

    # ---- 拼接所有段（不含 FileHeader）----
    payload = grph_section + head_section + spr_section + sprd_section + play_section + grou_section

    # ---- FileHeader ----
    # mapSize = 4(MAP magic) + payload 长度
    map_size = 4 + len(payload)

    file_header = b"RES "
    file_header += pack_uint32(map_size)
    file_header += b"MAP "

    # ---- 写入文件 ----
    full_data = file_header + payload
    output_path = Path(output_path)
    output_path.write_bytes(full_data)

    print(f"Written {len(full_data)} bytes to {output_path.name}")
    print(f"  sprites: {len(sprites)}")
    print(f"  mapSize field: {map_size}")


# ============================================================
# 偏置计算
# ============================================================

def calculate_shift(sprites_raw, map_size_x, map_size_y):
    """
    计算居中偏置量，并**强制吸附到编辑器的逻辑网格**。

    编辑器网格特征 (根据 room_sample.json 逆向)：
      posX ≡ 20 (mod 40)
      posY ≡ 14 (mod 28)
    """
    # 以 shift=0 算所有物理坐标
    all_x = []
    all_y = []
    for vid, gx, gy in sprites_raw:
        pos_x, pos_y = to_iso(gx, gy, 0.0, 0.0)
        if vid in (VID_WALL_B, 603, 612):
            pos_x -= 40.0
        all_x.append(pos_x)
        all_y.append(pos_y)

    min_x = min(all_x)
    max_x = max(all_x)
    min_y = min(all_y)
    max_y = max(all_y)

    width = max_x - min_x
    height = max_y - min_y

    # 理论上的纯数学居中偏置
    raw_shift_x = (map_size_x - width) / 2.0 - min_x
    raw_shift_y = (map_size_y - height) / 2.0 - min_y

    # 网格吸附：强制让 shiftX 满足 = K * 40 + 20，shiftY 满足 = J * 28 + 14
    grid_x = round((raw_shift_x - 20.0) / 40.0)
    shift_x = grid_x * 40.0 + 20.0

    grid_y = round((raw_shift_y - 14.0) / 28.0)
    shift_y = grid_y * 28.0 + 14.0

    return shift_x, shift_y


# ============================================================
# 主入口
# ============================================================

def main():
    """生成三个测试 .map 文件。"""
    print("=" * 60)
    print("Automapper Demo: 墙壁铺设公式验证")
    print("=" * 60)

    # ---- Test 1: 一排走向 A 墙壁 (/) ----
    print("\n--- Test 1: 走向 A (VID 601, /) 沿 gy 轴 ---")
    raw1 = [(VID_WALL_A, 0, gy) for gy in range(5)]
    shift_x, shift_y = calculate_shift(raw1, MAP_SIZE_X, MAP_SIZE_Y)
    print(f"  shiftX={shift_x}, shiftY={shift_y}")
    sprites1 = test1_wall_a(shift_x, shift_y)
    for s in sprites1:
        print(f"  VID={s['vid']}, pos=({s['posX']}, {s['posY']})")
    write_map(sprites1, OUTPUT_DIR / "test1_wall_a.map")

    # ---- Test 2: 一排走向 B 墙壁 (\) ----
    print("\n--- Test 2: 走向 B (VID 602, \\) 沿 gx 轴 ---")
    raw2 = [(VID_WALL_B, gx, 0) for gx in range(5)]
    shift_x, shift_y = calculate_shift(raw2, MAP_SIZE_X, MAP_SIZE_Y)
    print(f"  shiftX={shift_x}, shiftY={shift_y}")
    sprites2 = test2_wall_b(shift_x, shift_y)
    for s in sprites2:
        print(f"  VID={s['vid']}, pos=({s['posX']}, {s['posY']})")
    write_map(sprites2, OUTPUT_DIR / "test2_wall_b.map")

    # ---- Test 3: 5×5 方形房间 ----
    print("\n--- Test 3: 5x5 方形房间 ---")
    # 收集所有格点用于居中计算
    raw3 = []
    # 立柱
    for gx, gy in [(0, 0), (5, 0), (5, 5), (0, 5)]:
        raw3.append((VID_PILLAR, gx, gy))
    # 走向 A (601, /): 沿 gy 轴（墙壁延伸到角落，与立柱叠放）
    for gy in range(1, 6):
        raw3.append((VID_WALL_A, 0, gy))
        raw3.append((VID_WALL_A, 5, gy))
    # 走向 B (602, \): 沿 gx 轴
    for gx in range(1, 6):
        raw3.append((VID_WALL_B, gx, 0))
        raw3.append((VID_WALL_B, gx, 5))

    shift_x, shift_y = calculate_shift(raw3, MAP_SIZE_X, MAP_SIZE_Y)
    print(f"  shiftX={shift_x}, shiftY={shift_y}")
    sprites3 = test3_square_room(shift_x, shift_y)
    print(f"  total sprites: {len(sprites3)}")
    for s in sprites3:
        print(f"  VID={s['vid']}, pos=({s['posX']}, {s['posY']})")
    write_map(sprites3, OUTPUT_DIR / "test3_room.map")

    print("\n" + "=" * 60)
    print("Done! 用 MapEdit.exe 打开 .map 文件验证。")
    print("或者用 as1_map_unpack.py 解包检查结构。")
    print("=" * 60)


if __name__ == "__main__":
    main()

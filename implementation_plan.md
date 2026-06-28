# 门复合体（门框+门+指示灯）与挖墙逻辑的实现计划

我们已对 `D:\c coding\auto_mapper\_pre-demo\door\door_standard_wall_open.json` 以及 `.map` 底层数据进行了深度分析，发现以下核心物理装配规律：

1. **坐标完全重合**：门框、门扇、指示灯的 `posX` 和 `posY` **完全一致**。
2. **高度对齐**：门框和门扇的基准 `posZ = 0.0`。指示灯的 `posZ` 默认为 **10.0**。
3. **方向对齐（大门防穿透 Bug 规避）**：
   * **A方向门（纵向）**：门框和指示灯的 `direction = 128`，门扇的 `direction = 64`（刚好规避大门穿透 bug）。
   * **B方向门（横向）**：门框、门扇、指示灯的 `direction = 0`。
4. **VID 资产对齐（Standard Wall）**：
   * **大门框**：`608` | **小门框**：`606`
   * **大门开**：`607` | **大门关**：`611`
   * **小门开**：`605` | **小门关**：`617`
   * **指示灯**：绿灯 `423` | 红灯 `424` | 损坏灯 `425`

---

## User Review Required

> [!IMPORTANT]
> 1. **方向生成规则**：我们在 C++ 端会根据门的方向（A方向/B方向）自动对门框、门扇、指示灯设置对应的 `direction` 值（A方向为 128 / 64，B方向为 0），以此满足大门防穿透的规避法则。
> 2. **无门扇的激光门（Lab Wall）**：如果对应 `wall_type` 是实验舱（`WALL_TYPE_LAB = 1`），因为激光门在开启时激光柱消失，我们将不生成门扇 Sprite。

## Open Questions

无。

---

## Proposed Changes

### C++ Core & API Component

#### [MODIFY] [wall_builder.h](file:///d:/c%20coding/auto_mapper/src/auto_mapper/core/wall_builder.h)
- 引入门类型的状态定义与常量：
  * 指示灯状态：`LIGHT_STATE_GREEN = 0`, `LIGHT_STATE_RED = 1`, `LIGHT_STATE_BROKEN = 2`。
  * 门开启状态：`DOOR_STATE_CLOSED = 0`, `DOOR_STATE_OPEN = 1`。
- 定义 `DoorProfile` 结构体，描述特定墙面风格对应的门资产：
  ```cpp
  struct DoorProfile {
      int id_frame_small;
      int id_frame_large;
      int id_panel_small_open;
      int id_panel_small_closed;
      int id_panel_large_open;
      int id_panel_large_closed;
      int id_light_green;
      int id_light_red;
      int id_light_broken;
  };
  ```
- 定义 `DoorInstance` 结构体：
  ```cpp
  struct DoorInstance {
      GridPoint pos;
      int wall_type;     // 继承自墙面风格 (0=standard, 1=lab)
      int direction_type; // 0=A方向 (纵向), 1=B方向 (横向)
      int size;          // 1=小门, 2=大门
      int door_state;    // 0=关闭, 1=打开
      int light_state;   // 0=绿, 1=红, 2=损坏
      float z_offset;    // 用于可变死门高度，默认为0.0f
  };
  ```
- 修改 `WallBuilder::build` 接口，传入 `std::vector<DoorInstance>`。

#### [MODIFY] [wall_builder.cpp](file:///d:/c%20coding/auto_mapper/src/auto_mapper/core/wall_builder.cpp)
- **增加门资产配置**：
  * `WALL_TYPE_STANDARD`: `DoorProfile = { 606, 608, 605, 617, 607, 611, 423, 424, 425 }`
  * `WALL_TYPE_LAB`: `DoorProfile`（激光门无门扇，只留门框，灯 VID 置 0）
- **逻辑挖墙**：
  在 `process_wall_sprites` 提取普通墙（`edges_a` 和 `edges_b`）后：
  * 遍历 `doors`，如果门方向是 A方向（纵向），在 `edges_a` 中删除 `pos`，大门还需删除邻近格点。
  * 如果是 B方向（横向），在 `edges_b` 中进行相应删除。
  * 由于墙壁被抠空，两端会自动立起 Pillar。
- **装配门 Sprite**：
  在 `build` 管道末尾，计算门网格的 ISO 物理坐标 `(px, py)`，按下列规则插入 Sprite 列表：
  * **门框 Sprite**：
    * `vid` 根据 `size` 选用 `id_frame_small` 或 `id_frame_large`。
    * `posZ` = 0.0f。
    * `direction` = (direction_type == 0) ? 128 : 0。
  * **门扇 Sprite**：
    * `vid` 根据 `size` 和 `door_state` 从 `DoorProfile` 中选用。
    * `posZ` = 0.0f + `z_offset`。
    * `direction` = (direction_type == 0) ? 64 : 0。
  * **指示灯 Sprite**：
    * `vid` 根据 `light_state` 选用。
    * `posZ` = 10.0f（默认高度）。
    * `direction` = (direction_type == 0) ? 128 : 0。

#### [MODIFY] [api.h](file:///d:/c%20coding/auto_mapper/src/auto_mapper/api.h) & [api.cpp](file:///d:/c%20coding/auto_mapper/src/auto_mapper/api.cpp)
- 添加 `CDoor` 导出结构体，扩充 API，并在 API 中接收门数组。

---

### Python UI Component

#### [MODIFY] [main_ui.py](file:///d:/c%20coding/auto_mapper/src/python_layer/main_ui.py)
- 更新 `CDoor` 对应 ctypes 绑定。
- 在 `import_segments` / `export_segments` 中，集成 JSON `"doors"` 数组的导入导出支持，格式定义为：
  ```json
  "doors": [
    {
      "pos": { "x": 2, "y": 5 },
      "wall_type": 0,
      "direction_type": 0,
      "size": 2,
      "door_state": 1,
      "light_state": 2,
      "z_offset": 0.0
    }
  ]
  ```
- 修改 `generate_map` 调用，将 Python 的 `doors` 数组同步传入动态库生成。

---

## Verification Plan

### Automated Tests
1. 编译动态库：运行 `cmake` 和 `make` 生成新的动态库。
2. 运行生成命令：载入 `_ui_jsons/door_standard_wall_open.json` 测试用例，输出地图。
3. 验证输出的 `ui_output.map` 中的 Sprite 属性，确保坐标与 demo 完全对应。

### Manual Verification
- 检查大门的穿透 Bug 规避情况。
- 对比指示灯的渲染高度（$Z = 10.0$）和开门/关门 VID。

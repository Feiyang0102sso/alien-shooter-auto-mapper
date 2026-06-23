# 实现地板与天花板自动铺设 (Flood Fill方案)

该计划详述了如何通过“大水漫灌”（Flood Fill）算法区分建筑内部与外部，从而自动在墙体内部铺设可配置的地板，在墙体外部铺设泥土地天花板。

## Goal Description

基于网格坐标系，利用墙体的包围盒（Bounding Box）进行 Flood Fill。
- 蔓延到达的区域为 **外部 (OUTSIDE)**，铺设天花板材质。
- 无法到达的区域为 **内部 (INSIDE)**，根据设定铺设对应的地板材质。
- **不同对齐支持**：由于地板和天花板尺寸不一（如 500标准地板为 40x28，而 503大地板和 504天花板为 80x56，相当于 2x2 的标准格子），生成时将根据材质自身的跨度进行降频采样对齐（例如每 2x2 基础网格放置一个大 Sprite）。

## 确认的设计决策
根据讨论，采取**方案A（基于全图/多边形属性）**：
- 在 `Segment` 中增加 `int floor_type = 0;`。
- 如果某格子属于 `INSIDE`，我们将根据最近的墙壁或划定该区域的 Segment 来决定它的 `floor_type`。

## Proposed Changes

### 1. 材质配置层 (`auto_mapper/core/wall_builder.h`)

#### [MODIFY] `wall_builder.h`
新增地板和天花板配置，参考 JSON 提取的尺寸和高度数据：
```cpp
struct FloorProfile {
    int vid;
    float step_x, step_y;
    float pos_z;
};

struct CeilingProfile {
    int vid;
    float step_x, step_y;
    float pos_z;
};

// 预定义 Profile
constexpr FloorProfile FLOOR_STANDARD = {500, 40.0f, 28.0f, 0.0f};  // 占用 1x1 标准网格
constexpr FloorProfile FLOOR_LAB      = {503, 80.0f, 56.0f, 0.0f};  // 占用 2x2 标准网格

constexpr CeilingProfile CEILING_STANDARD = {504, 80.0f, 56.0f, 90.0f}; // 占用 2x2 标准网格，高度更高以遮挡墙壁
```
给 `Segment` 增加 `int floor_type = 0;`。

### 2. 网格拓扑与填充算法 (`auto_mapper/core/wall_builder.cpp`)

#### [MODIFY] `wall_builder.cpp`
- **步骤 A: Bounding Box 计算** 
  计算出完整的最小外包矩形，向外扩展两格形成物理边界。
- **步骤 B: 初始化网格图状态** 
  使用二维图，默认均为 `UNEXPLORED`。并且为每个格子记录它属于哪种 `floor_type`（可以通过离格子最近的墙体/Segment 赋值）。
- **步骤 C: 栅格化墙壁阻挡层** 
  将带有墙体的网格坐标标记为 `WALL_BLOCK`。
- **步骤 D: 洪水填充** 
  从最外围触发 BFS，凡能连通的非 `WALL_BLOCK` 格子标记为 `OUTSIDE`。
- **步骤 E: 生成地砖与天花板 Sprite (双重对齐逻辑)** 
  由于存在不同的 `step` 大小，我们在渲染时按材质的网格跨度（跨度=材质step/基础step）进行遍历对齐。
  - **天花板 (80x56跨度)**：以 2x2 为步长遍历 `OUTSIDE` 区域，放置 504。
  - **大地板 (80x56跨度)**：以 2x2 为步长遍历 `INSIDE` 区域，若 `floor_type` 是 LAB，放置 503。
  - **小地板 (40x28跨度)**：以 1x1 为步长遍历 `INSIDE` 区域，若 `floor_type` 是 STANDARD，放置 500。
  
  *对齐不严格的处理*：只要当前遍历点处于 `OUTSIDE` 或 `INSIDE`，就可以放置对应的大格子，即使大格子边缘可能会略微穿模，但这正是我们期望的“被墙体/天花板遮挡掩盖”。
- **步骤 F: Z-Order 及高度处理** 
  - 地板：`posZ = 0.0`
  - 天花板：`posZ = 90.0` (高过墙体)
  渲染列表组装顺序：地板块先添加，墙壁其次，最后添天花板。

## Verification Plan
### Automated Tests
- 在 `test_geometry.cpp` 中验证 Flood Fill 能否正确给出 `INSIDE` 和 `OUTSIDE` 集合。
- 验证大尺寸材质（如 80x56）生成时，坐标系跨度确实为 2 的整数倍。

### Manual Verification
- 生成地图，在 Alien Shooter 中确认 504 泥土高过墙体遮盖了外部。
- 确认实验室大房间中铺了 503，标准小房间铺了 500。

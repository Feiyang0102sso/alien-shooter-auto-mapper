# 引入 Floor 与 Ceiling 的 Divisor 支持及独立错位对齐方案

本计划旨在解决 Floor（地板）和 Ceiling（天花板）尚未支持 `grid_divisor` 细分网格的问题。我们将根据官方对齐机制与最新的逆向资源字符串线索，为其实现专用的对齐计算函数，并重构相关生成逻辑。

---

## 背景与理论支撑（官方资源字符串逆向）

根据 `MapEdit.exe` 资源字符串表（STRINGTABLE）证实，编辑器内部存在两种完全不同的对齐机制：

1. **墙体对齐（快捷键 `s`，资源 ID `40025`）**：
   *   俄语原文字符串：`"'s' - Переключение в/из режима поклеточного смещения курсора"`（进/出逐格移动光标模式）
   *   适用对象：**墙壁**、**立柱**、**独立物件**。
   *   实现方式：在 `s` 模式下采用正交网格几何中心对齐。

2. **地板/天花板对齐（资源 ID `40034`）**：
   *   俄语原文字符串：`"Переключение режима смещения всех нечетных строк на пол клетки"`（所有奇数行半格错位对齐模式）
   *   适用对象：**地板（Floor）** 与 **天花板（Ceiling）**，两者均需开启。
   *   实现方式：交错等轴测菱形网格，奇数行自动平移半个身位以实现无缝嵌套。

---

## 方案设计

### 1. 数据结构更新
在 `FloorProfile` 和 `CeilingProfile` 结构体中添加 `grid_divisor` 字段。
*   `FLOOR_STANDARD`: `grid_divisor = 1`
*   `FLOOR_LAB`: `grid_divisor = 1` (无法进一步细分)
*   `CEILING_STANDARD`: `grid_divisor = 2` (物理尺寸 80x56，细分后为 40x28，移去原有硬编码的 `shift_offset`)

### 2. 独立对齐算法 `get_floor_ceiling_shift`与全局原点
在 `wall_builder.cpp` 中实现专用于地砖类（Floor/Ceiling）的交错网格对齐函数。
为了防止不同地砖类型（例如 FLOOR_LAB 的 divisor=1 与 CEILING_STANDARD 的 divisor=2）因为计算公式中 `divisor` 差异而算出不同的原点偏移，导致天花板与地板出现半个格子的错位（超出或缺失），所有地板和天花板的铺设逻辑在计算 Shift 偏置时，应**统一并仅使用标准的 40x28 网格规格**（即 `step_x = 40.0f, step_y = 28.0f, divisor = 1`）算出一个全局一致的 `base_shift`。
该函数支持细分网格，并在 Y 轴上根据 X 轴网格索引的奇偶性进行动态半步长交错：
```cpp
static MapPoint get_floor_ceiling_shift(float map_size_x, float step_x, float step_y, int grid_divisor) {
    float divisor = static_cast<float>(grid_divisor);
    float grid_step_x = step_x / divisor;
    float grid_step_y = step_y / divisor;

    float half_step_x = grid_step_x / 2.0f;
    float half_step_y = grid_step_y / 2.0f;

    int n = static_cast<int>(std::round((map_size_x / 2.0f - half_step_x) / grid_step_x));
    float shift_x = n * grid_step_x;

    bool n_is_even = (n % 2 == 0);
    float shift_y = n_is_even ? (grid_step_y + half_step_y) : half_step_y;

    return {shift_x, shift_y};
}
```

### 3. 铺设逻辑重构
*   **`place_floors`**: 使用 `get_floor_ceiling_shift` 计算对齐偏置，并在等轴测投影 `to_iso` 时保持采用原始物理网格步长 `step_x`/`step_y`（仅在 `shift` 偏置应用细分结果），以防止重叠铺设。
*   **`place_ceilings`**: 同样使用 `get_floor_ceiling_shift` 计算对齐偏置，在反向投影计算坐标边界范围以及在正向投影 `to_iso` 生成物理坐标时，均保持采用原始物理步长 `step_x`/`step_y`（仅在 `shift` 中应用细分），使天花板按原本物理尺寸正确拼接。

---

## Proposed Changes

### [auto_mapper]

#### [MODIFY] [wall_builder.h](file:///d:/c%20coding/auto_mapper/src/auto_mapper/core/wall_builder.h)
*   在 `FloorProfile` 中添加 `int grid_divisor = 1;` 字段。
*   在 `CeilingProfile` 中添加 `int grid_divisor = 1;` 字段。
*   更新 `FLOOR_STANDARD`、`FLOOR_LAB` 和 `CEILING_STANDARD` 的 constexpr 静态定义。

#### [MODIFY] [wall_builder.cpp](file:///d:/c%20coding/auto_mapper/src/auto_mapper/core/wall_builder.cpp)
*   更新 `get_floor_profile` 和 `get_ceiling_profile` 里的静态查找表，匹配新的 `grid_divisor` 参数并移去硬编码的 `shift_offset`。
*   实现 `get_floor_ceiling_shift` 静态辅助函数。
*   重构 `place_floors`：使用 `get_floor_ceiling_shift` 和细分步长进行投影。
*   重构 `place_ceilings`：使用 `get_floor_ceiling_shift` 和细分步长进行范围反推与投影。

---

## Verification Plan

### Automated Tests
运行现有的单元测试，确保原有标准墙体与地板的对齐逻辑未被破坏：
```powershell
# 在构建目录中运行测试
ctest --output-on-failure
```

### Manual Verification
生成 `test_output.map` 并使用 `MapEdit.exe` 打开，观察：
*   标准地板是否闭合且对齐。
*   实验室地板（FLOOR_LAB）是否完美吸附并与墙体对齐，无缝隙或重叠。
*   天花板（CEILING_STANDARD）是否精确覆盖在对应地板的垂直上方。

# 引入 Floor 与 Ceiling 的 Divisor 支持及独立错位对齐方案

本计划旨在解决 Floor（地板）和 Ceiling（天花板）尚未支持 `grid_divisor` 细分网格的问题。我们将根据官方对齐机制与最新的逆向资源字符串线索，为其实现专用的对齐计算函数，并重构相关生成逻辑。

---

## 背景与理论支撑（官方资源字符串逆向）

根据 `MapEdit.exe` 资源字符串表（STRINGTABLE）证实，编辑器内部存在两种完全不同的对齐机制：

1. **墙体对齐（快捷键 `s`，资源 ID `40025`）**：
   *   俄语原文字符串：`"'s' - Переключение в/из режима поклеточного смещения cursora"`（进/出逐格移动光标模式）
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
为了防止不同地砖类型（例如 FLOOR_LAB 的 divisor=1 与 CEILING_STANDARD 的 divisor=2）因为计算公式中 `divisor` 差异而算出不同的原点偏移，导致天花板与地板出现半个格子的错位（超出或缺失），所有地板 and 天花板的铺设逻辑在计算 Shift 偏置时，应**统一并仅使用标准的 40x28 网格规格**（即 `step_x = 40.0f, step_y = 28.0f, divisor = 1`）算出一个全局一致的 `base_shift`。
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

#### [MODIFY] [map_writer.h](file:///d:/c%20coding/auto_mapper/src/auto_mapper/io/map_writer.h)
*   为 `Sprite` 结构体添加默认构造函数和一个带默认参数的构造函数，避免在代码中进行冗长的逐成员赋值初始化。

#### [MODIFY] [wall_builder.h](file:///d:/c%20coding/auto_mapper/src/auto_mapper/core/wall_builder.h)
*   在 `FloorProfile` 和 `CeilingProfile` 中添加 `int grid_divisor` 并更新默认常量定义。
*   新增公有的或私有的 `place_single_floor`、`place_single_ceiling`、`place_single_wall` 辅助方法。

#### [MODIFY] [wall_builder.cpp](file:///d:/c%20coding/auto_mapper/src/auto_mapper/core/wall_builder.cpp)
*   实现 `get_floor_ceiling_shift` 独立对齐函数。
*   提炼并实现 `place_single_floor`、`place_single_ceiling`、`place_single_wall`，分别负责生成单个瓦片/墙体的 Sprite。
*   重构 `place_floors` 和 `place_ceilings`，让它们计算好范围后，循环调用上述 `place_single` 方法生成 Sprite。

#### [NEW] [test_wall_builder.cpp](file:///d:/c%20coding/auto_mapper/tests/test_wall_builder.cpp)
*   在 `tests/test_wall_builder.cpp` 中新增 `FloorCeilingManualGoldAlignment` 测试用例，动态加载 `_pre-demo/floor_celling_only.json` 提取三种瓦片的基准参考点，并通过调用 `place_single` 检验生成瓦片的数学对齐属性，确保完美兼容手动 Gold。



---

## Verification Plan

### Automated Tests
*   运行新增的 GTest 校验：
    ```powershell
    # 编译并运行测试
    .\builder.bat
    ```

### Manual Verification
*   将生成的 `test_output.map` 载入地图编辑器 `MapEdit.exe` 中，直接肉眼检查对齐，并确认与手动铺设效果（`_pre-demo/floor_celling_only.json` 对应的原始地图）无差异。

---

## 4. 表征 Grid 对齐保证与测试用例设计

### A. 如何保证铺设完整按“表征 Grid”对齐
为了防止地板/天花板瓦片（如 80x56）在计算中因为细分网格（如 40x28 甚至 cell_size=5）而被错误地放置在半步长偏移位置，或产生重叠，我们必须在代码和测试中强制执行以下几何约束：

1.  **平铺生成步长约束**：
    *   在 `place_floors` 和 `place_ceilings` 循环生成坐标时，迭代步长必须是**表征网格步长**（`step_x`, `step_y`），不能是除以 `grid_divisor` 后的值。
    *   即使有细分（如天花板的 `grid_divisor = 2`），细分只影响边界范围的反推 and 对齐偏置（`shift`），但迭代步长依然保持 $80.0 \times 56.0$，以确保物理上每个 Sprite 占据完整的独立网格。

2.  **边界判定逻辑（点/多点判定）**：
    *   将瓦片的中心点 `(px, py)` 映射到 `cell_size = 5` 的物理格栅时，由于使用的是整块瓦片尺寸进行步进，一旦该中心点通过判定（位于房间内），就输出整块不可分割的瓦片。
    *   避免在生成阶段对瓦片本身做任何“亚网格级别（最小 grid）”的微调，保证其物理位置绝对符合离散等轴测变换公式。

### B. 单元测试用例（Test Cases）与基准 Gold 对准设计

由于铺设算法仅要求“铺满”整个房间，而具体的房间边界与墙面几何可能存在允许的少量偏差（即测试不应严格卡死生成了多少个瓦片或绝对的边界范围），我们应当**重点测试铺设的格点对齐与瓦片间距是否正确**。

为此，我们将直接使用用户手动铺设的地图 `_pre-demo/floor_celling_only.json` 作为**基准 Gold**。

#### 用例验证流程：
1.  **加载基准 Gold 文件**：
    *   在 C++ 单元测试中，我们读取并解析 `_pre-demo/floor_celling_only.json` 中的 Sprite 坐标。
    *   对于每种瓦片类型（`vid = 500` 标准地板，`vid = 503` 实验室地板，`vid = 504` 标准天花板），我们从中提取任意一个手动铺设的有效 Sprite 坐标作为**网格基准参考点 $(ref_x, ref_y)$**。例如：
        *   `vid = 500` -> $(ref_{500x}, ref_{500y}) = (320.0, 210.0)$
        *   `vid = 503` -> $(ref_{503x}, ref_{503y}) = (1040.0, 700.0)$
        *   `vid = 504` -> $(ref_{504x}, ref_{504y}) = (360.0, 462.0)$

2.  **构建测试区域并运行程序绘制**：
    *   在测试中声明一个闭合墙体构成的房间，运行 `WallBuilder::build` 动态生成地板与天花板 Sprite 列表。

3.  **计算相对位移与网格整点校验（对齐校验）**：
    *   对于程序生成的每一个 Sprite $(p_x, p_y)$：
        *   计算其到对应类型的 Gold 基准参考点 $(ref_x, ref_y)$ 的相对物理位移：
            $$d_x = p_x - ref_x$$
            $$d_y = p_y - ref_y$$
        *   代入等轴测逆变换公式，求出两个瓦片之间的**逻辑网格差值** $(g_{xdiff}, g_{ydiff})$：
            $$g_{xdiff} = \frac{\frac{d_x}{step_x} + \frac{d_y}{step_y}}{2}$$
            $$g_{ydiff} = \frac{\frac{d_y}{step_y} - \frac{d_x}{step_x}}{2}$$
    *   **Gold 校验断言**：
        *   断言 $g_{xdiff}$ and $g_{ydiff}$ 必须是**严格的整数**（即与最接近的整数 `std::round` 的偏差小于 $10^{-4}$）：
            `EXPECT_NEAR(g_xdiff, std::round(g_xdiff), 1e-4);`
            `EXPECT_NEAR(g_ydiff, std::round(g_ydiff), 1e-4);`
        *   如果通过，说明程序自动生成的瓦片**完美落在了用户手动铺设所确立的标准网格整点上**。

4.  **无重叠性校验**：
    *   将所有生成的同一 `vid` 的 Sprite 计算出的 $(g_{xdiff}, g_{ydiff})$ 存入 `std::set`。
    *   **断言条件**：`set` 的大小必须等于该类 Sprite 的总数量，确保程序没有在同一个对齐位置重叠铺设多个地砖。

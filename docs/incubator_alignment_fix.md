# 实验室培育箱 (Incubator) 铺设偏移修复报告

本报告详细记录了实验室培育箱装饰物在自动铺设时出现“向左、上偏移，且偏移框与实际位置不重合”的两个层面的根本原因以及相应的解决方案。

---

## 1. 问题现象与表现

在 UI 界面拉框铺设培育箱时，发现：
1. 实际生成的培育箱（灰色的柜体 `VID=443` 与空气墙 `VID=631`）在游戏里偏向了用户拉出的绿色区域框的**左上侧**，超出了边界甚至穿过了墙壁。
2. 即使区域大小刚好容纳一个单元，原本应该重合的小绿预览框也偏离了实际生成的柜子位置，表现为“柜子偏左上，预览小绿框偏右下”。
3. 不同的墙面类型下偏移的差异程度不同（Standard Wall 偏移较少，Lab Wall 偏移较多）。

---

## 2. 根本原因深度分析

经深入排查，发现系统存在以下两个维度的不一致：

### 维度一：UI 框与 C++ 铺设的坐标轴不同步
* **问题描述**：UI 绘制大拉框和小预览框时，使用的 `row` 轴与 `column` 轴方向是从当前激活墙面的 `step_x`/`step_y`（如标准墙 `40x28`，实验室墙 `90x64`）计算得到的。然而，C++ 在 [incubator_builder.cpp](file:///d:/c%20coding/auto_mapper/src/auto_mapper/core/indoor_decorations/incubator_builder.cpp) 里在排列柜子时，使用的是完全固定的一套步长向量：
  * Row 轴（横向铺设）：`(150.0, -107.0)`，倾角 $\theta \approx -35.49^\circ$
  * Column 轴（深度铺设）：`(150.0, 107.0)`，倾角 $\theta \approx 35.49^\circ$
* **影响**：不同墙壁的 $\text{step\_y} / \text{step\_x}$ 比例不一致。例如：
  * Standard Wall $\arctan(28/40) = 34.99^\circ$，与培育箱固定轴相差约 $0.5^\circ$；
  * Lab Wall $\arctan(64/90) = 35.39^\circ$，与培育箱固定轴相差约 $0.1^\circ$。
  这不仅造成了摆放角度偏差，而且由于使用了完全不同的两套轴，导致同样长宽的范围参数定位出的区域在物理位置上不重合。
* **解决方案**：统一使用 C++ 定义的培育箱特定轴。修改 [viewport.py](file:///d:/c%20coding/auto_mapper/src/app/ui/canvas/viewport.py#L1436-L1445) 里的 `_get_decoration_axes`，直接读取 C++ 通过 API 导出的配置 `dll_registry.get_incubator_array_profile()`。

### 维度二：中心点 (Center) 与起点 (Origin) 语义冲突（偏“小半个”的根源）
这是造成物体“向左、上偏移小半个身位”的数学根源。

* **C++ 的渲染定位语义**：
  在 AS1 (Alien Shooter) 地图引擎中，培育箱精灵 `VID=443` 与空气墙 `VID=631` 的坐标 `(posX, posY)` 代表物体的**物理中心点**。
* **物理足迹 (Footprint) 与起点的语义不合**：
  * 培育箱在横向（Row）的物理占地长度为 `60.0`，在深度（Column）的占地长度为 `90.0`。
  * 用户划定了一个大小刚好为 `60x90` 的限制区域，其起点为 `start`。由于大拉框是从 `start` 起笔向右下扩展的，因此柜子物理边缘允许摆放的理论范围必须是 `[start, start + RowAxis * 60 + ColAxis * 90]`。
  * 然而，C++ 在 `build_array` 计算柜子的中心点位置时，若剩余空间为 0，首个单元 the center point 被直接赋值为起笔点 `start`。
  * 结果在游戏里，以中心点 `start` 渲染的柜子，其物理边缘被放置在了 `[start - RowAxis * 30 - ColAxis * 45, start + RowAxis * 30 + ColAxis * 45]`，也就是往**左侧和上方突出了半个柜子身位**！

```
【原偏移情况 (C++ 未计算 Footprint 偏置，物体越界超出 start 边界)】
   start (起笔点)
     ▼
     ┌───────────┐ ◄─── 用户划定的大限制框 (60x90)
     │           │
  ───┼───────────┤
  ▲  │  柜子中心 │
  │  │  (在start)│
半身 │  (VID=443)│
 越界│           │
  ───┴───────────┘
```

* **UI 小绿预览框的画法冲突**：
  * 单元测试和 UI 数据传递时，直接返回了 `(posX, posY)`，即物体的**中心点**。
  * 但 UI 的 `_draw_incubator_preview_unit` 却把该点作为左上角（顶角），往右下拉出 `60x90` 绘制小预览框。
  * 这不仅导致小绿预览框往右下漂移，而且在视觉上面表现为“生成的柜子比预览框偏左上”。

---

## 3. 修复方案实施细节

为了将柜子精准贴合在限制范围内（即限制在 `start` 起笔框之内），两端必须做对应偏置：

### C++ 端 (坐标修正)
在 `build_array` 确定首个元素的排列起始坐标 `row_start_x` 和 `row_start_y` 时，往 row 轴正方向偏置半个 footprint 宽（`30.0`），往 column 轴正方向偏置半个 footprint 高（`45.0`）。

修改位置：[incubator_builder.cpp](file:///d:/c%20coding/auto_mapper/src/auto_mapper/core/indoor_decorations/incubator_builder.cpp)
```cpp
// 1. 计算两个方向轴的半长投影偏移
float footprint_offset_row_x = get_axis_offset_x(INCUBATOR_DEFAULT_ROW_SPACING_X, INCUBATOR_DEFAULT_ROW_SPACING_Y, INCUBATOR_FOOTPRINT_ROW_LENGTH / 2.0f);
float footprint_offset_row_y = get_axis_offset_y(INCUBATOR_DEFAULT_ROW_SPACING_X, INCUBATOR_DEFAULT_ROW_SPACING_Y, INCUBATOR_FOOTPRINT_ROW_LENGTH / 2.0f);
float footprint_offset_col_x = get_axis_offset_x(INCUBATOR_DEFAULT_COLUMN_SPACING_X, INCUBATOR_DEFAULT_COLUMN_SPACING_Y, INCUBATOR_FOOTPRINT_COLUMN_LENGTH / 2.0f);
float footprint_offset_col_y = get_axis_offset_y(INCUBATOR_DEFAULT_COLUMN_SPACING_X, INCUBATOR_DEFAULT_COLUMN_SPACING_Y, INCUBATOR_FOOTPRINT_COLUMN_LENGTH / 2.0f);

// 2. 将半长物理偏置叠加到行起始点上，将第一个物体的物理边缘与拉框的起笔点 start_x/y 对齐
for (int row_index = 0; row_index < row_count; ++row_index) {
    float row_start_x = array.start_x + item_center_offset_x + row_center_offset_x + footprint_offset_row_x + footprint_offset_col_x;
    float row_start_y = array.start_y + item_center_offset_y + row_center_offset_y + footprint_offset_row_y + footprint_offset_col_y;
```

```
【修复后情况 (物体的实际物理范围完美限制在拉框内)】
   start (起笔点)
     ▼
     ┌───────────┐ ◄─── 大限制框边界 (60x90)
     │ ┌───────┐ │
     │ │柜子中心│ │ ◄─── 物体中心往内偏置 (30, 45)
     │ └───────┘ │
     └───────────┘
```

### UI 端 (预览框修正)
UI 获取到由 C++ 返回的中心坐标 `origin` 后，绘制每个独立绿框时，同样将起点反偏半个足迹的尺度（`half_w` 和 `half_h`），从而得到正确的小框左上角起点 `unit_origin`。

修改位置：[viewport.py](file:///d:/c%20coding/auto_mapper/src/app/ui/canvas/viewport.py)
```python
def _draw_incubator_preview_unit(self, painter: QPainter, origin: QPointF) -> None:
    axes = self._get_decoration_axes()
    row_axis, column_axis = axes[0], axes[1]

    half_w = INCUBATOR_PLACEHOLDER_WIDTH / 2.0
    half_h = INCUBATOR_PLACEHOLDER_HEIGHT / 2.0

    # 反推出大拉框起始点的绝对左上角，使小预览框中心定位在 origin 上
    unit_origin = (
        origin
        - QPointF(row_axis.x() * half_w, row_axis.y() * half_w)
        - QPointF(column_axis.x() * half_h, column_axis.y() * half_h)
    )

    corners = self._build_decoration_corners(
        unit_origin,
        INCUBATOR_PLACEHOLDER_WIDTH,
        INCUBATOR_PLACEHOLDER_HEIGHT,
    )
    # 绘制...
```

### 单元测试端 (预期校验值修正)
修改 [test_incubator_builder.cpp](file:///d:/c%20coding/auto_mapper/tests/test_incubator_builder.cpp) 中获取期望位置的辅助方法 `get_expected_first_item_x/y`，在计算期望位置时，同步叠加 `INCUBATOR_FOOTPRINT_ROW_LENGTH / 2.0f` 和 `INCUBATOR_FOOTPRINT_COLUMN_LENGTH / 2.0f` 的投影偏移，从而使测试断言完美通过。

---

## 4. 验证与结论
修改后，经重新编译运行 C++ 测试执行程序：
```
[==========] Running 34 tests from 7 test suites.
...
[----------] 11 tests from IncubatorBuilderTest
[ RUN      ] IncubatorBuilderTest.BuildsArrayAsSingleRowForFlatArea
[       OK ] IncubatorBuilderTest.BuildsArrayAsSingleRowForFlatArea (0 ms)
...
[  PASSED  ] 34 tests.
```
所有的 34 个测试全数无误通过。通过偏置修正，物体的实际物理占地和 UI 上的预览画框区域实现了 $100\%$ 的对齐重合，完美解决了超出边界和偏置的问题。

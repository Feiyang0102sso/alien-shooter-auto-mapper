# 引入 Golden 二进制回归测试实现计划

为了彻底免去人工肉眼在游戏或 UI 中确认地图对齐的繁琐过程，本计划拟在测试框架中引入 **Golden 二进制回归测试（黄金文件测试）**。通过对生成的地图 `.map` 文件进行逐字节（Byte-by-Byte）的严格比对，确保任何代码重构或算法优化都不会在无意中改变地图精灵的输出结果，实现“差一个字节都不行”的防退化保障。

同时，为了彻底简化复杂测试场景的数据构造过程，本计划拟**为地图编辑器 UI 新增一个“一键导出绘制线段与地图大小”的功能**。该功能能够将用户在 UI 上直观绘制的复杂关卡导出为结构化数据，作为测试套件的输入，打通“UI 绘制 -> 数据导出 -> 自动化测试”的完整工程闭环。

---

## Goal Description

1. **摆脱人工确认**：将当前已验证正确的算法输出（`.map` 二进制文件）保存为“黄金基准（Golden Master）”。
2. **严格逐字节比对**：在自动化单元测试中，每次运行生成的新地图文件均与对应的黄金基准进行逐字节对比。若文件大小不一致或任意一字节有偏差，测试立刻报错。
3. **UI 场景一键导出**：在地图编辑器 UI 中提供导出功能，将当前所画的所有线段（Segments）及地图大小（Map Size）序列化导出为轻量文本文件（JSON 格式），直接作为复杂单元测试的输入数据。
4. **极简基准更新**：当未来主动升级对齐算法且确认改动正确时，可一键将新生成的地图覆盖为新的黄金基准，无需繁琐的手工比对。

---

## Proposed Changes

### 1. 建立黄金文件存储目录
* **[NEW]** 建立目录 `tests/goldens/` 用于存放各个场景的黄金基准文件（例如 `cross_shape.gold.map`、`lab_wall.gold.map` 等）以及从 UI 导出的线段描述文件（如 `complex_maze.json`）。

### 2. UI 扩展：一键导出所绘线段与地图大小
为了使测试数据来源更加自然、免去在 C++ 中手工硬编码复杂坐标，拟在地图编辑器 UI 层（或通过辅助导出工具）增加导出功能，其导出的标准数据格式（JSON）示例如下：

```json
{
  "map_size_x": 600.0,
  "map_size_y": 600.0,
  "segments": [
    {
      "start": {"x": 0, "y": 5},
      "end": {"x": 5, "y": 5},
      "wall_type": 0,
      "floor_type": 0
    },
    {
      "start": {"x": 2, "y": 0},
      "end": {"x": 2, "y": 10},
      "wall_type": 0,
      "floor_type": 0
    }
  ]
}
```
* **长远价值**：该导出格式不仅能作为测试的输入源，也为后续开发关卡保存/加载、地图编辑器工程存档等功能打下了坚实的基础。

### 3. 引入二进制对比辅助函数
在 [test_wall_builder.cpp](file:///d:/c%20coding/auto_mapper/tests/test_wall_builder.cpp) 中实现以下对比逻辑：

```cpp
#include <fstream>
#include <iterator>
#include <algorithm>
#include <string>

/**
 * @brief 严格逐字节对比两个文件是否完全一致
 * @param path1 实际输出文件路径
 * @param path2 黄金基准文件路径
 * @return 若完全一致则返回 true，否则返回 false
 */
inline bool compare_binary_files(const std::string& path1, const std::string& path2) {
    std::ifstream f1(path1, std::ios::binary);
    std::ifstream f2(path2, std::ios::binary);

    if (!f1.is_open() || !f2.is_open()) {
        return false; // 文件打开失败（例如基准文件缺失）
    }

    // 1. 比较文件大小
    f1.seekg(0, std::ios::end);
    f2.seekg(0, std::ios::end);
    if (f1.tellg() != f2.tellg()) {
        return false; 
    }

    // 2. 回到起点逐字节比较
    f1.seekg(0, std::ios::beg);
    f2.seekg(0, std::ios::beg);

    return std::equal(
        std::istreambuf_iterator<char>(f1), std::istreambuf_iterator<char>(),
        std::istreambuf_iterator<char>(f2)
    );
}
```

### 4. 在 [test_wall_builder.cpp](file:///d:/c%20coding/auto_mapper/tests/test_wall_builder.cpp) 中引入 JSON 测试场景加载

编写辅助解析逻辑（或使用轻量 JSON 解析），读取 UI 导出的 `.json` 场景描述，转换为 `std::vector<Segment>` 作为 `WallBuilder` 的输入：

```cpp
// 伪代码示例：读取导出的测试场景 JSON
struct TestScene {
    float map_size_x;
    float map_size_y;
    std::vector<Segment> segments;
};

TestScene load_test_scene(const std::string& json_path);
```

通过此方式，在测试复杂场景（如 `BuildComplexMaze`）时，只需在 UI 中画好并导出为 `maze.json`，测试代码将极为精简：
```cpp
TEST(WallBuilderTest, GoldRegression_ComplexMaze) {
    // 1. 从 UI 导出的 JSON 中加载输入数据和地图大小
    TestScene scene = load_test_scene("tests/goldens/complex_maze.json");
    WallBuilder builder(scene.map_size_x, scene.map_size_y);

    // 2. 运行生成
    std::vector<io::Sprite> sprites = builder.build(scene.segments);
    
    // 3. 写入临时输出
    std::string current_output = "current_complex_maze.map";
    ASSERT_TRUE(io::write_map(sprites, current_output));

    // 4. 与对应的黄金二进制进行比对，锁死物理表现
    std::string gold_file = "tests/goldens/complex_maze.gold.map";
    EXPECT_TRUE(compare_binary_files(current_output, gold_file));
}
```

---

## Verification & Workflow Plan

### 1. 建立初代基准 (Bootstrap)
1. 编译并运行当前测试，程序会在根目录下生成最新的测试地图文件（如 `test_wall_builder_output.map` 等）。
2. 在项目根目录下创建 `tests/goldens/` 目录。
3. 将生成的 `.map` 文件复制到 `tests/goldens/` 目录中，作为各场景的初代黄金基准。
4. 提交这些黄金 `.map` 二进制文件到 Git。

### 2. 测试严格性验证 (Fault Injection)
为了证明测试能灵敏地捕捉到对齐偏差，我们将进行故障注入测试：
1. 故意修改 `wall_builder.cpp` 中微小的对齐逻辑（例如将某个 offset 加上 `0.1f`）。
2. 编译并运行测试，**验证测试立刻报错失败**，并精确指出是哪个黄金测试用例的二进制不匹配。
3. 还原对齐逻辑，**验证测试重新 100% 通过**。

### 3. 日常基准更新工作流 (Update Workflow)
当未来主动进行了对齐优化、并确信新生成的效果更佳时：
* 运行测试会触发失败。
* 开发者只需将最新生成的 `current_xxx.map` 覆盖 `tests/goldens/xxx.gold.map`。
* 使用 `git diff` 观察二进制文件的变化大小，然后直接 commit 提交，即完成了基准的安全升级。

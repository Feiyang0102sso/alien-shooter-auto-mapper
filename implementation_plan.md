# 重构墙体生成测试并引入 Golden 回归测试

为了提供更高质量的防退化保障，本计划重构 `tests/test_wall_builder.cpp`。我们将使用用户已在 `tests/golden/` 目录下准备好的黄金测试数据（`wall_builder.gold.json` 和 `wall_builder.gold.map`）进行完全的二进制回归测试，并删除旧有的、无意义的零散测试。

为了保持测试架构的优雅与轻量，所有通用的辅助逻辑（如文件逐字节比对、极简 JSON 解析等）将以 **Header-only** 的形式实现在新文件 `tests/utils/test_utils.h` 中，免去修改 CMake 配置的繁琐。

---

## Goal Description

1. **废弃旧测试**：删除 `tests/test_wall_builder.cpp` 中现有的 `BuildCrossShapeStandard` 等琐碎且意义不大的单元测试。
2. **引入 Header-only 工具库**：新建 [test_utils.h](file:///d:/c%20coding/auto_mapper/tests/utils/test_utils.h)，包含：
   - 极其稳健且轻量的专用 JSON 扫描解析器，将 `wall_builder.gold.json` 解析为地图大小与线段数组。
   - `compare_binary_files` 函数，对生成的 `.map` 二进制文件进行严格的逐字节比对。
3. **Golden 回归测试**：在 `tests/test_wall_builder.cpp` 中编写全新的测试用例，从 JSON 加载线段，调用 `WallBuilder::build`，将结果写入临时文件 `current_wall_builder.map`，与 `wall_builder.gold.map` 逐字节比对。
4. **自动清理临时文件**：比对完成后（无论成功与否，最好在测试析构/RAII 中，或者在验证后），主动删除测试生成的临时文件，确保工作区干净。

---

## Proposed Changes

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

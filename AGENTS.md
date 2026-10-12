# auto_mapper 项目 Agent 说明

## 项目定位

`auto_mapper` 是一个 Alien Shooter 风格 `.map` 自动生成器。

@CONTEXT.md

项目分成两层，全部是 C++20，一起编译成一个 `AutoMapper.exe`：

- `src/auto_mapper/`：核心算法，静态库 `auto_mapper_core`。
- `src/editor/`：Qt Widgets 编辑器，直接使用核心的 C++ 类型和函数，没有中间的 C API 层。

旧的 Python/PySide6 编辑器（`src/app/`）已删除，需要对照时查 git 历史（`5334e04 v1.0` 及之前）。
仓库里剩下的 Python 只有离线数据工具：`tools/decoration_pipeline/` 与 `_prepare/`，使用 `.venv`。

Agent 修改代码时，先判断改动属于哪一层。坐标、VID、墙体、门、装饰物生成规则优先放在核心；UI 只负责交互、预览、项目数据保存和调用核心生成地图。

## 当前架构重点

- 整张地图的生成入口是 `src/auto_mapper/core/map_generator.*` 的 `generate_map(MapRequest)`。
- 装饰物 stamp profile 通过 `core/decoration_builder/decoration_registry.*` 查询。
- 墙体生成核心是 `src/auto_mapper/core/wall_builder.*`。
- 门生成核心是 `src/auto_mapper/core/door_builder.*`。
- 地图写出逻辑是 `src/auto_mapper/io/map_writer.*`。
- 室内装饰物在 `src/auto_mapper/core/indoor_decorations/`。

编辑器 `src/editor/` 的目录沿用旧 Python 包的分法：

| 位置 | 职责 |
|---|---|
| `main.cpp` | 启动：日志、首次语言选择、QSS、主窗口 |
| `config.*` | `AutoMapper.cfg` 读写、嵌入资源注册 |
| `logger.*` | Qt 日志转发到核心 spdlog，与核心共用 exe 旁的 `AutoMapper.log` |
| `i18n/locale.*`、`i18n/text_keys.h` | 唯一的文案入口 `Locale::tr()` 与固定文案 key |
| `catalog/catalog.*` | 墙体套装、部件、装饰系列；几何来自核心 profile，元数据来自 `resources/catalog.json` |
| `project/project.*` | 项目数据、JSON 读写、调用生成器 |
| `hotkeys/` | 快捷键表（唯一数据源）、分发、帮助对话框 |
| `ui/colors.*` | 所有代码绘制用的颜色 |
| `ui/main_window.*` | 组装面板、画布、快捷键，执行编辑器命令 |
| `ui/canvas/viewport.*` | 画布：坐标换算、绘制、鼠标交互 |
| `ui/panels/` | 左侧货架（`left_shelf`、`theme_shelf`）、右侧属性面板（`inspector`、`as1_ceiling`） |
| `ui/tools/` | 绘制模式、左侧工具栏、橡皮擦几何与属性 |
| `resources/` | `catalog.json`、`i18n/*.json`、`styles/*.qss`，编译进 exe |

预览图在仓库根目录 `assets/preview/`，不编译进 exe：构建和打包时复制到 exe 旁的 `assets/`，
运行时经 `Catalog::imagePath()` 按 exe 目录查找。

## 坐标系统

这个项目最容易出错的是坐标语义。不要把这几种坐标混在一起：

- Grid 坐标：用户在 UI 中画墙用的逻辑格点，例如 `(gx, gy)`。
- Map/Physical 坐标：游戏 `.map` 文件里的物理坐标，例如 sprite 的 `posX` / `posY`。
- Screen 坐标：Qt 画布像素坐标，受 zoom、pan、居中偏移影响。

墙体从 Grid 转 Physical 时，依赖墙体 profile 的 `step_x`、`step_y`、offset 和 `grid_divisor`。

Standard 墙：

- wall type: `0`
- `/` 墙 VID: `601`
- `\` 墙 VID: `602`
- pillar VID: `604`
- step: `40 x 28`
- `dir_b` 需要 `offset_b_x = -40`

Lab 墙：

- wall type: `1`
- `/` 墙 VID: `651`
- `\` 墙 VID: `650`
- pillar VID: `652`
- step: `90 x 64`
- `dir_a` 需要 `offset_a_x = 90`
- `grid_divisor = 2`

Standard Dark 墙：

- wall type: `2`
- `/` 墙 VID: `620`
- `\` 墙 VID: `621`
- pillar VID: `622`
- step: `40 x 28`

如果要改墙体对齐、地板、天花板、门挖空，先读 `wall_builder.h` 里的 profile，再读对应测试和 golden 文件。

## 核心与编辑器的边界

编辑器直接包含核心头文件，没有 C API 层：

- 项目数据的墙体和门直接是 `core::Segment` / `core::DoorInstance`（`project/project.h`）。
- 生成地图：`Project::generate()` 组装 `core::MapRequest`，调用 `core::generate_map()`。
  墙套 1 的门同步墙体类型、墙套 2–9 的门强制 2 格、Lab 门挖空 1 格等规则都在 `map_generator.cpp`，不要在 UI 里重复。
- 墙体几何读 `WallBuilder::get_wall_profile()`（经 `Catalog::wallProfile()`）；装饰物 stamp 读 `decoration_registry`。
- 不要在编辑器里直接包含 `decoration_profiles_as2.h`：profile 数据是 `static` 定义，每个包含它的编译单元都会复制一份。
- 不要让 UI 复制核心的布局常量，直接引用核心头文件里的常量。

## 配色与多语言

- 代码绘制的颜色（画布、墙体/门图例）只能来自 `ui/colors.h`；墙体类型到颜色的映射只有 `colors::wallColor()`。
- 控件外观（按钮、面板、标签）只写在 `resources/styles/dark_theme.qss`，不要在代码里 `setStyleSheet` 写颜色。
- 所有可见文字经 `Locale::tr()`。固定文案用 `TextKey::` 常量；墙体、部件、装饰系列、快捷键的文案按 id 拼 key
  （如 `wall.<id>.label`、`decoration.stamp.<id>.label`、`hotkey.<command>`）。
- `resources/i18n/en_US.json` 与 `zh_CN.json` 必须有完全相同的 key 集合，`localesShareOneKeySet` 测试会检查。
- `catalog.json` 只放与语言无关的数据（id、预览路径、nvid 文本、部件的 `label_key`），不放文案和颜色。
- 唯一例外：首次启动的语言选择框和“语言 language”按钮故意中英双语。

## 装饰物阵列

`14c8091 add universal array builder for decorations` 之后，装饰物阵列的共享核心是：

- `src/auto_mapper/core/indoor_decorations/array_layout.h`
- `src/auto_mapper/core/indoor_decorations/array_layout.cpp`

`calculate_array_item_anchors()` 负责计算阵列中每个装饰单元的中心点，返回顺序是 row-major。

现有使用者：

- `incubator_builder.*`
- `desk_builder.*`

修改 incubator 或 desk 阵列时，优先复用 `ArrayLayout`，不要在 builder 里重新写一套阵列循环。

Incubator 关键常量：

- body VID: `443`
- air wall VID: `631`
- big computer VID: `135`
- item axis: `(150, -107)`
- row axis: `(150, 107)`
- footprint item length: `60`
- footprint row length: `90`

Desk 关键常量：

- desk body VID: `404`
- broken computer VID: `411`
- enabled computer VID: `410`
- chair VID: `407`
- 默认 row/column spacing: `(-120, 80)` 和 `(-120, -80)`

装饰物坐标语义：C++ 生成的 `pos_x` / `pos_y` 是装饰物中心点，不是 UI 预览框左上角。UI 绘制单元预览框时，需要根据 footprint 反推框的起点。

## UI 与项目数据

项目数据当前保持简单 JSON 兼容结构：

- `segments`
- `doors`
- `decorations`
- `map_size_x`
- `map_size_y`
- `is_door_open`

`IncubatorDecoration` 存的是物理坐标区域：

- `start_x`
- `start_y`
- `row_length`
- `column_length`
- `item_spacing_scale`
- `row_spacing_scale`

不要把装饰物区域改成 Grid 坐标，除非同时重做 UI 交互、保存格式、`MapRequest` 输入和测试。

UI 画布规则：

- 鼠标输入从 Screen 反算到 Map，再反算到 Grid。
- 墙体和门主要跟 Grid 坐标绑定。
- 装饰物区域主要跟 Physical 坐标绑定。
- active wall profile 会影响网格边界、颜色、预览和可绘制部件。

装饰物交互只有三种互斥状态，判定入口是 `Viewport::isPlacing()` 和
`Viewport::isEditing()`，改交互前先读这两个方法：

- 放置：装饰物工具已装载（`activeStamp` / `activeArray` 非空）。绿色虚线 ghost 跟随光标，
  左键一律放新的，不做 hit test，因此允许重叠放置。放置后不选中刚放的装饰物。
- 编辑：绘制模式是 `DrawingMode::Select` 且没有装载装饰物工具。不画 ghost，
  左键选中并拖动已有装饰物，点空白取消选中。
- 关闭：墙体与橡皮工具。装饰物只是背景，点击一律交给墙体逻辑。

放置模式下右键退出到编辑模式。橙色高亮只表示"选择工具正在操作它"，
不允许和 ghost 同屏——这是当初把两种状态揉在一个变量里踩过的坑。

注意 `Viewport::resetInteraction()` 会把 `part` 还原成 `PART_WALL_BODY`。
装载装饰物工具时该字段被置空，不还原的话之后墙体工具画不出任何东西。

工具栏按钮跟着左侧货架模式走，入口是 `MainWindow::chooseShelf()` 和 `MainWindow::syncMode()`：

- Wall Sets 货架：显示橡皮，隐藏 Select。
- Decoration Sets 货架：显示 Select，隐藏橡皮。
- 每个可隐藏的工具在 `DrawingToolbar::separators_` 里绑定了自己的分隔线，`setModeVisible()` 一起显隐。

隐藏当前选中的工具会留下一个死状态，所以 `chooseShelf()` 必须同时换到
一个还看得见的工具：橡皮换 Select，Select 换 Polyline，切回墙体货架时顺带卸下装饰物工具。

快捷键只在 `hotkeys/hotkey_config.cpp` 的表里定义，帮助对话框和分发都读这张表；
焦点在输入框、数字框、下拉框时不触发（`HotkeyManager::acceptsShortcut()`）。

## 测试与 Golden 文件

核心测试在 `tests/test_*.cpp`，编辑器测试在 `tests/editor/test_editor.cpp`（交互、旧 JSON 兼容、四种输出、中文路径、
多语言 key 集合、快捷键分发）。

重点测试：

- `tests/test_array_layout.cpp`
- `tests/test_incubator_builder.cpp`
- `tests/test_desk_builder.cpp`
- `tests/test_wall_builder.cpp`
- `tests/test_door_builder.cpp`
- `tests/test_map_writer.cpp`

Golden 文件在 `tests/golden/`。改动导致 golden 输出变化时，必须能解释是预期行为变化，不要静默更新。

当前测试路径工具集中在：

- `tests/utils/test_utils.h`

新增测试时，优先复用这里的路径辅助函数，不要把当前工作目录假设写死在测试里。

## 构建与运行

编辑器使用 MSVC + Qt 6.11.1（Qt Base、Qt Image Formats）。
只维护 `msvc-release` 一个预设；Qt SDK 路径在 `CMakePresets.json` 的 `CMAKE_PREFIX_PATH`。

CMake 分四个文件：根 `CMakeLists.txt` 只放全局设置、依赖（spdlog、gtest、Qt）和三个 `add_subdirectory`；
`src/auto_mapper/` 放核心静态库，`src/editor/` 放编辑器、exe、部署与打包，`tests/` 放核心测试、编辑器测试和 golden 导出工具。
所有 exe 都输出到 `build/msvc-release/` 同一目录，共用 Qt DLL 和 `assets/`。
`builder.bat` 构建并运行核心与原生 UI 测试；
CLion 运行目标为 `auto_mapper_editor`。`pack_release.bat` 先清空 `build/` 里旧的 `AutoMapper V *` 目录和 zip，再生成 `build/AutoMapper V x.y.z/` 与同名 zip。

版本号只写在 `src/editor/editor.rc` 的 `AUTO_MAPPER_VERSION_TEXT` / `AUTO_MAPPER_VERSION_NUMBER` 两行（三段式，两行一起改）；exe 文件属性、`--version`、日志和打包名都从这里来，CMake 不管版本。
具体 SDK 配置和独立运行方式见 README 的“Windows 开发构建（MSVC）”。
原生测试详细结果保存在对应构建目录的 `native-editor-results.txt`。

C++ 构建由 CMake 管理，测试目标是：

```text
auto_mapper_tests
```

运行离线 Python 工具（`tools/decoration_pipeline/`、`_prepare/`）前先启用虚拟环境：

```powershell
.\.venv\Scripts\Activate.ps1
```

打包相关文件：

- `pack_release.bat`
- `src/editor/CMakeLists.txt` 里的 `install()` 规则
- `AutoMapper.cfg`（运行时语言配置）

不要在没有确认的情况下改打包配置或 `release/` 输出。

## 编码偏好

本项目优先可读、直接、少抽象。

- C++：保持 C++20，避免炫技式模板和过度压缩表达式。
- Python（仅离线工具）：优先 `pathlib`，避免复杂推导式、lambda、三元表达式。
- 日志：必要时写简短英文日志，方便运行时定位。
- 注释：保留已有注释。新增注释只解释不明显的坐标、VID、偏移、兼容性原因。
- 异常：不要无脑加 `try-catch` / `try-except`。让错误在合适层级暴露。

## 修改前检查清单

改墙体：

- 读 `wall_builder.h`
- 核对 wall profile、offset、grid divisor
- 跑 wall / door / map writer 相关测试

改门：

- 读 `door_builder.*`
- 核对 active/dead/lab laser 的状态语义
- 检查 AS1 全局开门开关（`Project::doorsOpen`）是否受影响

改装饰物：

- 先看 `array_layout.*`
- 再看具体 builder
- UI 预览必须和 C++ preview points 或 profile 对齐

改生成流程或核心类型：

- 同步 `src/editor/` 调用处与 `tests/test_map_generator.cpp`
- 编辑器测试 `legacyFixturesKeepGeneratedContent` 会把 golden 项目生成的 `.map` 逐字节比对

改 UI 文案或颜色：

- 文案两份 locale 一起改，颜色只改 `ui/colors.h` 或 QSS

改 UI 坐标：

- 明确当前变量是 Screen、Map/Physical 还是 Grid
- 不要用视觉上“看起来对”的偏移替代坐标语义

## Git 约束

- 开始前看 `git status --short --branch`。
- 不要还原用户未说明要还原的改动。
- 不要使用 `git reset --hard`、`git checkout --` 等破坏性命令，除非用户明确要求。
- 提交时保持单一主题。

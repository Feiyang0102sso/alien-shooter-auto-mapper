# auto_mapper 项目 Agent 说明

## 项目定位

`auto_mapper` 是一个 Alien Shooter 风格 `.map` 自动生成器。

@CONTEXT.md

项目分成两层：

- `src/auto_mapper/`：C++20 核心与 DLL，对外暴露 C API。
- `src/app/`：Python/PySide6 编辑器 UI，通过 `ctypes` 调用 C++ DLL。

Agent 修改代码时，先判断改动属于哪一层。坐标、VID、墙体、门、装饰物生成规则优先放在 C++；UI 只负责交互、预览、项目数据保存和调用 DLL。

## 当前架构重点

- C++ 入口是 `src/auto_mapper/api.h` 和 `src/auto_mapper/api.cpp`。
- Python DLL 客户端是 `src/app/binding/client.py`。
- Python 绑定结构体在 `src/app/binding/structures.py`。
- 项目数据结构在 `src/app/project/data.py`。
- 主画布是 `src/app/ui/canvas/viewport.py`。
- 墙体生成核心是 `src/auto_mapper/core/wall_builder.*`。
- 门生成核心是 `src/auto_mapper/core/door_builder.*`。
- 地图写出逻辑是 `src/auto_mapper/io/map_writer.*`。
- 室内装饰物在 `src/auto_mapper/core/indoor_decorations/`。

## 坐标系统

这个项目最容易出错的是坐标语义。不要把这几种坐标混在一起：

- Grid 坐标：用户在 UI 中画墙用的逻辑格点，例如 `(gx, gy)`。
- Map/Physical 坐标：游戏 `.map` 文件里的物理坐标，例如 sprite 的 `posX` / `posY`。
- Screen 坐标：PySide 画布像素坐标，受 zoom、pan、居中偏移影响。

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

## DLL API 约束

Python UI 依赖 C++ DLL 的 C API。当前 Python 绑定要求：

```python
REQUIRED_API_VERSION = 4
```

只要改了这些内容，就必须同步检查 C++ API、Python ctypes 结构体和客户端签名：

- `src/auto_mapper/api.h`
- `src/auto_mapper/api.cpp`
- `src/app/binding/structures.py`
- `src/app/binding/client.py`

新增或修改导出函数时：

- C++ 结构体字段顺序必须和 Python `ctypes.Structure` 完全一致。
- 如果破坏兼容性，更新 `get_auto_mapper_api_version()` 返回值和 Python 的 `REQUIRED_API_VERSION`。
- 不要让 UI 复制 C++ 的核心布局常量。能从 DLL 暴露 profile 的，优先从 DLL 读取。

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

不要把装饰物区域改成 Grid 坐标，除非同时重做 UI 交互、保存格式、DLL 输入和测试。

UI 画布规则：

- 鼠标输入从 Screen 反算到 Map，再反算到 Grid。
- 墙体和门主要跟 Grid 坐标绑定。
- 装饰物区域主要跟 Physical 坐标绑定。
- active wall profile 会影响网格边界、颜色、预览和可绘制部件。

## 测试与 Golden 文件

C++ 测试在 `tests/test_*.cpp`。

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

C++ 构建由 CMake 管理，测试目标是：

```text
auto_mapper_tests
```

Python 项目需要先启用虚拟环境：

```powershell
.\.venv\Scripts\Activate.ps1
```

Python 包入口是：

```text
auto-mapper = app.main:main
```

打包相关文件：

- `pack_nuitka.py`
- `pack_nuitka.bat`
- `AutoMapper.cfg`

不要在没有确认的情况下改打包配置或 release/dist 输出。

## 编码偏好

本项目优先可读、直接、少抽象。

- C++：保持 C++20，避免炫技式模板和过度压缩表达式。
- Python：优先 `pathlib`，避免复杂推导式、lambda、三元表达式。
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
- 检查 Python `is_door_open` 全局开关是否受影响

改装饰物：

- 先看 `array_layout.*`
- 再看具体 builder
- UI 预览必须和 C++ preview points 或 profile 对齐

改 DLL API：

- 同步 C++ API、Python ctypes 结构、client 配置
- 必要时升级 API version

改 UI 坐标：

- 明确当前变量是 Screen、Map/Physical 还是 Grid
- 不要用视觉上“看起来对”的偏移替代坐标语义

## Git 约束

- 开始前看 `git status --short --branch`。
- 不要还原用户未说明要还原的改动。
- 不要使用 `git reset --hard`、`git checkout --` 等破坏性命令，除非用户明确要求。
- 提交时保持单一主题。

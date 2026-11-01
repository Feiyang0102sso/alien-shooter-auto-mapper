# auto_mapper 项目 Agent 说明

## 项目定位

`auto_mapper` 是一个 Alien Shooter 风格 `.map` 自动生成器。

@CONTEXT.md

项目分成两层，全部是 C++20，一起编译成一个 `AutoMapper.exe`：

- `src/auto_mapper/`：核心算法，静态库 `auto_mapper_core`。
- `src/editor/`：Qt Widgets 编辑器，直接使用核心的 C++ 类型和函数，没有中间的 C API 层。

旧的 Python/PySide6 编辑器（`src/app/`）已删除，需要对照时查 git 历史（`5334e04 v1.0` 及之前）。
仓库里剩下的 Python 只有离线数据工具 `_prepare/`，使用 `.venv`。

Agent 修改代码时，先判断改动属于哪一层。坐标、VID、墙体、门、装饰物生成规则优先放在核心；UI 只负责交互、预览、项目数据保存和调用核心生成地图。

## 当前架构重点

- 整张地图的生成入口是 `src/auto_mapper/core/map_generator.*` 的 `generate_map(MapRequest)`。
- 装饰物 stamp 由 `core/decoration_builder/decoration_library.*` 在启动时从库地图注册（AS2 系列按 region，AS1 按固定网格），见下文“装饰物 Stamp”。
- 墙体生成核心是 `src/auto_mapper/core/wall_builder.*`。
- 曲线墙（Curve Wall）核心是 `core/wall_builder/curve_wall_builder.*`，见下文“曲线墙”。
- 散布笔刷都在 `core/scatter/`：公共的散布引擎 `scatter_fill.*` 直接放在这一层；
  物资箱（Loot Box）笔刷与掉落表在 `core/scatter/loot/`，见下文“物资箱”；
  尸体血迹（Gore）笔刷在 `core/scatter/gore/`，见下文“尸体血迹”。
- 门生成核心是 `src/auto_mapper/core/door_builder.*`。
- 地图写出逻辑是 `src/auto_mapper/io/map_writer.*`。
- 室内装饰物在 `src/auto_mapper/core/indoor_decorations/`。
- 游戏数据读取在 `src/game_assets/`（静态库 `auto_mapper_game_assets`）：`objects.res`、`Vid/*.vid` 单帧解码。
  只有编辑器的装饰物贴图预览使用它，生成 `.map` 不依赖它。

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
| `ui/canvas/stamp_sprites.*` | 装饰物成员贴图缓存（经 `game_assets` 解码，叠加库地图 gamma） |
| `ui/panels/` | 左侧货架（`left_shelf`、`theme_shelf`）、右侧属性面板（`inspector`、`as1_ceiling`、`loot_brush`、`gore_brush`）、贴图预览小窗口（`sprite_preview_dialog`）、掉落表对话框（`loot_table_dialog`） |
| `ui/tools/` | 绘制模式、左侧工具栏、橡皮擦几何与属性 |
| `resources/` | `catalog.json`、`i18n/*.json`、`styles/*.qss`，编译进 exe |

预览图在仓库根目录 `assets/preview/`，装饰物库地图在 `assets/decoration_maps/`，都不编译进 exe：
构建和打包时复制到 exe 旁的 `assets/`，运行时按 exe 目录查找。

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
- 墙体几何读 `WallBuilder::get_wall_profile()`（经 `Catalog::wallProfile()`）；装饰物 stamp 读 `Catalog` 持有的 `DecorationLibrary`，
  生成时经 `MapRequest::decoration_library` 传给核心。
- 不要让 UI 复制核心的布局常量，直接引用核心头文件里的常量。

## 配色与多语言

- 代码绘制的颜色（画布、墙体/门图例）只能来自 `ui/colors.h`；墙体类型到颜色的映射只有 `colors::wallColor()`。
- 控件外观（按钮、面板、标签）只写在 `resources/styles/dark_theme.qss`，不要在代码里 `setStyleSheet` 写颜色。
- 所有可见文字经 `Locale::tr()`。固定文案用 `TextKey::` 常量；墙体、部件、装饰系列、快捷键的文案按 id 拼 key
  （如 `wall.<id>.label`、`hotkey.<command>`）。装饰物系列卡片直接显示库地图文件名，条目用 `label.stamp_region` 拼出；
  卡片描述读 `decoration.<文件名>.description`，没写就用 `label.stamp_series_description_default`；
  随仓库发布的 AS1 / AS2 库地图都要写好两份 locale 的描述，`resourcesAndCoreMetadata` 测试会逐个检查。
- `resources/i18n/en_US.json` 与 `zh_CN.json` 必须有完全相同的 key 集合，`localesShareOneKeySet` 测试会检查。
- `catalog.json` 只放与语言无关的数据（id、预览路径、nvid 文本、部件的 `label_key`），不放文案和颜色。
- 唯一例外：首次启动的语言选择框和“语言 language”按钮故意中英双语。

## 装饰物 Stamp

Stamp 不再有手写 profile 和预览图，全部来自库地图：`assets/decoration_maps/AS2/*.map`（官方 AS2 编辑器做的）
和 `assets/decoration_maps/AS1/*.map`（AS1 编辑器里手工修补过的，源头在 AS1 游戏目录 `Maps/_decoration/`）。
两个游戏族的 nvid 互不相干，所以是两个独立的 `DecorationLibrary`，`Catalog` 的 stamp 查询都带 `as1` 参数，按项目所属游戏族选库。

- `decoration_library.*`：按 HEAD 里的地图版本分两种读法，`0x0C` 走 AS1 网格，其余走 AS2 region。
- AS2 系列：读 SPR 和 SPRD。nvid `99` 的 sprite 是 region，SPR 坐标是 stamp 原点，
  SPRD 末尾 80 字节的 `+16` / `+20` 是宽高（Map 坐标轴对齐矩形）。其余 sprite 按 `(posX, posY)`
  落进哪个矩形归哪个 stamp，存相对原点的偏移；army、gamma、scale、z、direction 原样保留。
  Stamp id 是 `<文件名>:<序号>`，序号按 region 在 SPR 里的顺序从 1 开始；库地图里删改 region 会改变后面的序号。
- AS1：objects.res 没有 region 对象，stamp 摆在固定网格上：每行 6 格，每格 700 x 520，第一格左上角 `(400, 400)`，
  常量在 `decoration_library.cpp` 开头。每个非空格子是一个 stamp，序号是格子编号 `行 * 6 + 列 + 1`
  （和库地图预览图上的数字一致），删掉一格不影响其它格的编号。AS1 SPR 记录 28 字节，没有 gamma 和 scale，成员取默认值。
  选择框是成员锚点包围盒四周各加 40，框中心就是 stamp 原点；网格外的 sprite 打 warning 并忽略。
- 项目 JSON 存 stamp id；打开含未知 id（含另一个游戏族的 id）的项目会直接报错。
- `decoration_builder.*`：`build_stamp()` 只做平移。army 用库地图里的值，因为血迹等贴花在 `vid_armies_as2.h` 里没有绑定。
- `placeholder_vids.*`：画布占位符黑名单（血迹、阴影、`TERRAIN:` 类、书本等密集杂物），
  没有游戏目录读不到 objects.res，所以 nvid 写死在这里；AS1 与 AS2 系列各有一套完整名单（nvid 相同也分开列），
  三个查询函数都按 `io::MapFormat` 选名单，AS2/AS2R/AS2C/AS2OE 共用 AS2 名单；AS1 的血迹、弹壳、碎片、428 小物品不画 X，尸体照画；`lies_on_ground()` 标出贴地类，贴图预览先画它们；
  `is_invisible_block()` 标出 `BLOCK:` 空气墙，`stampDrawOrder()` 直接滤掉，既不画贴图也不画 X，生成的 `.map` 仍保留。
- 工具栏“贴图预览”打开一个非模态小窗口（`ui/panels/sprite_preview_dialog.*`），设置都写进 `AutoMapper.cfg`：
  `as2_path` / `as1_path`（AS2 / AS1 游戏目录，各自驱动本游戏族的装饰物贴图；画布持有两份 `StampSprites`，按项目游戏族取用）、
  `stamp_crosses_with_sprites`（有贴图时仍画绿色 X，0/1，默认 0）、`stamp_always_show_frame`（始终画选择框，0/1，默认 0）、
  `stamp_sprite_opacity`（贴图不透明度百分比滑条，默认 100，超出 `config.h` 的 10–100 按上下限取值；拖动时实时预览，松手才写 cfg）。
  窗口只发信号，由 `MainWindow` 存 cfg 并更新画布。
- 有游戏目录时成员用真实贴图画在 `(posX, posY - posZ)`，叠加库地图里的 gamma 和 scale，贴地类先画、其余按 posY 排序；
  z ≥ 5 的立物（叠放的柜子、桌上的电脑）继承最近的更低立物（锚点曼哈顿距离 60 内）的 posY，同深度按 z 自下而上，
  抬高的贴地类（尸体上的血）最后画。只按 posY 会让上层柜子被下层盖住，按 posY + posZ 又会把高物压到前面的植物上；
  解不出贴图的成员和没有游戏目录时回退成绿色 X。逻辑在 `ui/canvas/stamp_sprites.*` 与 `Viewport::drawStampMembers()`。
  半透明（放置 ghost 0.55、不透明度滑条 < 100）时先把整个 stamp 不透明地画进离屏图层再整体淡化，
  逐个贴图淡化会让叠放的下层透出上层。
- 选择框（AS2 是 region 框）默认只在放置 ghost（绿色虚线）、选中/拖动（橙色）和超出地图（红色）时画，见 `Viewport::drawDecoration()`。
- 贴图预览只用于装饰物，墙体（含曲线墙）、门、地板不画贴图（网格和墙体美术有半格偏差，未解决）。
- 库地图之外的 sprite 会打 warning 并忽略；空 region 跳过但仍占一个序号。
- 装饰物货架在 AS1 和 AS2 工程里都可用，各自只显示本游戏族的库地图。

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
  - 网格墙卡：显示折线 / 直线 / 矩形，隐藏手绘。
  - 曲线墙卡（`Viewport::isCurveWallCard()`）：只显示手绘和橡皮，`activateMode()` 拒绝网格工具。
- Decoration Sets 货架：显示 Select，隐藏橡皮和手绘。
- Loot 货架（物资箱）和 Gore 货架（尸体血迹），只在 AS2 系列工程显示：只显示手绘（即笔刷）和橡皮，
  `activateMode()` 拒绝 Select 和网格工具；判断入口是 `Viewport::isBrushCard()`。
  货架模式是 `ShelfMode`（`left_shelf.h`）四态：Walls / Decorations / Loot / Gore。
  货架按钮排成两行（`QGridLayout`）：第一行墙壁套装 / 装饰套装，第二行物资箱 / 尸体血迹；AS1 工程隐藏第二行。
- 每个可隐藏的工具在 `DrawingToolbar::separators_` 里绑定了自己的分隔线，`setModeVisible()` 一起显隐。

隐藏当前选中的工具会留下一个死状态，所以 `chooseShelf()` 必须同时换到
一个还看得见的工具：橡皮换 Select，Select 换 Polyline，切回墙体货架时顺带卸下装饰物工具。

快捷键只在 `hotkeys/hotkey_config.cpp` 的表里定义，帮助对话框和分发都读这张表；
焦点在输入框、数字框、下拉框时不触发（`HotkeyManager::acceptsShortcut()`）。

## 曲线墙

AS2 系列专用的“曲线墙”卡（`catalog.json` 里 id `100`，条目 `id: "curve_wall"`，用 `curve_wall_type` 区分）：
2670 山体、2700 异形巢穴（32 方向）、3300 悬崖（16 方向）。direction 都用满 0..255，配套的空气墙由引擎自己 Link，不写。

- 坐标：控制点是 Map 坐标，不走 Grid，项目 JSON 存在顶层 `curve_walls`（`curve_wall_type`、`closed`、`points`）。
  AS1 工程不能含曲线墙；生成 AS1 时核心会跳过并打 warning。
- 核心 `build_curve_wall()`：Catmull-Rom 平滑 → 按弧长等距取点 → `direction = 切线角（Map y 除以 0.7）+ 素材偏移`。
  间距和偏移来自官方地图统计（AS2R `Level_13` / `Level_14`，`Level_04` / AS2R `Level_08`）：
  2670 间距 110、偏移 0°；2700 间距 130、偏移 −27°；3300 间距 185、偏移 +56°。army：2670 / 2700 是 2，3300 是 0。
- 单面素材（`CurveWallProfile::one_sided`，目前只有 3300）：官方每条链的方向都只取一侧，不能按 128 折叠；
  按画线方向算，层状面在画线方向的左侧、裂纹面在右侧（贴图渲染核对过）。画布和图例在层状面一侧画短刻线，绘制中的笔迹也画。勾选“随机方向”时每片在切线方向上 ±4 单位抖动，不勾选时输出确定。
  每片贴图沿曲线要伸出锚点很远（地面长度：2670 约 125、2700 约 155，按贴图实测），
  所以开放曲线的首尾两片各往里缩 `end_reach`（换算成 Map 弧长时竖直方向乘 0.7），让岩石在游戏里正好止于画出的端点；
  画布上的线仍画到端点，代表墙的实际范围。太短放不下两片时只在中点放一片。闭环不缩。
- 编辑器：手绘工具（`DrawingMode::Freehand`，快捷键 5）按住拖动画笔迹，松手时用 `simplify_stroke()` 按屏幕 5 像素容差抽稀；
  松手时离起点 14 像素内就闭合。橡皮在曲线墙卡上按橡皮圆（与预览圆同一半径）切掉当前类型曲线的对应一段
  （`erase_curve_wall_circle()`）：曲线断成两段、闭环被切开，剩下的每段保留原控制点、端点落在圆上，
  短于半个片间距的残段一并删掉。
- `curveWallType` 不在 `resetInteraction()` 里清掉，否则在手绘和橡皮之间切换会丢掉曲线墙卡；
  `setWall()` / `armStamp()` / `armArray()` / `armLoot()` / `armGore()` / `chooseShelf()` 负责清掉它。
- 地板（勾选“地板”时）：曲线墙参与 `WallBuilder` 的物理网格（`build()` 的 `curve_walls` 参数，生成器在 AS1 时传空）。
  只铺被围住的区域，开放曲线只当边界、不沿线铺砖（`curve_line_grid` 上的格子不参与采样）。
  地板类型先从曲线向外扩散：能连到曲线墙的空地都铺曲线地板（岩石圈住房间时整个环形区是岩石地板），
  剩下的再按网格墙就近规则；没有曲线墙时结果与以前逐字节相同。
  2670 → 1121（与墙套 3 / 4 同一 profile，80×56，六值随机）；2700 → 510、3300 → 512（官方 132×94 网格，0–255 随机）。
  随机只在勾选“随机方向”时生效，否则方向为 0。floor type 用 100–102，避开墙体类型编号。
- 画布：曲线墙属于墙体，和网格墙一样只画线（同一个 `WALL_LINE_WIDTH`，绘制中的笔迹用网格墙预览的黄色虚线），
  另加每片的锚点小圆点，不画游戏贴图。编辑器预览与生成共用核心函数，不在 UI 里重写采样。

## 物资箱（Loot Box）

AS2 系列专用的第三个货架，三张卡：木箱 1225、金属箱 2332、异形蛋 2750（可混入破损蛋 2751）。
数据来自 AS2R 原版地图（`AlienShooter2 Reloaded\Maps`，AS2C 目录下的地图删过箱子，不可信）和 AS2C 的蛋。

- 掉落文本：写在 SPRD 每条记录末尾的 nameText（`io::Sprite::name_text`，writer 原样写出，空串时输出与以前逐字节相同）。
  一行是空格分隔的道具名（`EMPTY` 表示不掉），箱子被打碎时由游戏从这一行里随机挑一个；
  重复的名字就是权重。工具不读 `Weapon.cfg`、不解析也不校验道具名。AS2R 的蛋 nameText 是 quest 标记 `m1`，不是掉落。
- 掉落表（`core/scatter/loot/loot_table.*`）：每张卡一张表，每行是“一整条文本 + 概率”，概率之和 ≤ 100%，剩下的不写。
  `generate_map()` 在最后一步按 VID 对**所有**输出 sprite 抽一行写入，所以装饰物 stamp 里的箱子也会带上；破损蛋不在表里。
  超过 100% 的表直接让生成失败。每次导出重新抽，没有种子。
- 摆放（`core/scatter/loot/loot_box_builder.*`）：编辑器每落一笔调用 `scatter_loot_boxes()`，项目里存的是具体实例（`loot_boxes`）。
  - 一笔就是一堆：笔刷圆心先放一个，再把圆内的随机点按离圆心由近到远逐个尝试，离已放的（含已有的）≥ 间距才保留，
    所以箱子和蛋都从中心向外长出一团。参数只有笔刷半径、间距、叠放概率（箱子）、破损概率（蛋）；
    默认箱子半径 80、间距 45（官方相邻箱子 38–53），蛋半径 150、间距 35（`default_loot_brush()`）。
  - 每个地面箱子按叠放概率再叠一层（木箱 +25、金属箱 +30，水平偏移 ≤ 30）。默认叠放 15%（官方箱子叠箱子只有 1.4%，
    这里有意多一点），蛋默认 10% 破损。
  - 方向只取官方地图出现过的值：木箱 9 个、金属箱 8 个（`LOOT_*_BOX_DIRECTIONS`）。这些方向其实是**不同款式**的箱子，
    不是旋转；2332 的 objects.res 写着 10 方向，但 204 / 230 没有帧，所以不能按 `noDir` 推算。蛋用满 0..255。
  - army：箱子和活蛋 2，破损蛋 0（`vid_armies_as2.h` 里没有这几个 VID，由 builder 写）。
- 编辑器：`Viewport::lootKind` 非空即 Loot 卡，手绘拖动时每移动半个笔刷半径落一笔，落到地图外的箱子丢掉；
  橡皮按橡皮圆删当前卡的箱子（蛋卡连破损蛋一起删）。`lootKind` 和 `curveWallType` 一样不在 `resetInteraction()` 里清，
  由 `setWall()` / `armStamp()` / `armArray()` / `armCurveWall()` / `armGore()` / `chooseShelf()` 清。
  画布把所有箱子当成一个 stamp 的成员交给 `drawMembers()`，叠放顺序、半透明、贴图和叉号都与装饰物一致。
- 笔刷参数每张卡一份，只在本次会话里记住（`MainWindow::lootBrushes_`）。
  掉落表对话框是暗色的（`#lootTableDialog` QSS），文本列放宽，长文本在框内换行、框向下长高，不横向拉长；
  粘贴进来的换行会变成空格（nameText 只能是一行）。掉落表存在项目 JSON 的 `loot_tables`
  （`wood` / `metal` / `egg` → `[{text, percent}]`）。
- 掉落预设（`editor/loot_presets.*`）存在 exe 旁的 `loot_presets.ini`（工作目录里有同名文件时优先，同 `AutoMapper.cfg`），
  用 SimpleIni 读写。预设不分卡片、只按节名区分，任何卡都能载入。
  仓库里的模板是根目录的 `loot_presets.ini`（`[初期]` / `[中期]` / `[后期]` 取自 AS2R 第 1–5 / 6–10 / 11–15 关
  官方木箱、金属箱最常用的整条文本，只保留钞票 `Obj_money1-3`（不含金条 `Obj_money4`）、弹药和血量拾取物，概率按比例放大到
  官方空箱率；`[异形蛋]` 是 AS2C 蛋的那一行，节名可以随意改）。每次构建都用 `copy_if_different` 同步到 exe 旁（会覆盖存进构建目录那份的预设），
  `install()` 把它装到打包目录的 exe 旁。文件不存在时没有预设，第一次保存时创建。改模板只改这个 ini，不写进 C++。
  注意工作目录优先：以仓库根目录为工作目录运行编辑器时，读写的就是仓库里这份模板。
  格式：`[预设名]` 下 `itemN = 整条文本`、`itemN_p = 24.3%`（`%` 可省），按 N 排序、编号可断；缺文本或概率的行跳过并打 warning。
  保存只重写这一节（保留位置和注释、`item1..N` 连续编号），其他节和注释不动；节名不能含 `[` `]`。
  文件经 QFile 读、QSaveFile 写，不用 SimpleIni 的 `LoadFile()`（`fopen` 打不开中文路径）。
- 项目 JSON：`loot_boxes`（`vid`、`x`、`y`、`z`、`direction`）和 `loot_tables` 只在非空时写；AS1 工程出现 `loot_boxes` 直接报错。

## 尸体血迹（Gore）

AS2 系列专用的第四个货架，四张卡：骨头堆 1532、怪物尸体、士兵尸体、血滩。尸体和血迹一起生成，因为官方地图里
96–99% 的尸体旁边都有血。数据来自 AS2R 原版 17 张图（`AlienShooter2 Reloaded\Maps`）和 AS2C 原版 12 张图。

- 尸体池（`core/scatter/gore/gore_builder.h` 开头的常量）：怪物 23 个、人类 5 个，池内等概率抽；
  Boss、女王、机械、坦克、具名 NPC、碎片、`HIDE:` 版士兵（1157，与 MAPEDIT 版 1159 同贴图）不收。
  每个 VID 都在两个游戏目录里解码核对过有帧。
- **AS2C 特有尸体**：Conscription 的 objects.res 比 AS2R 多 4019 / 4023 / 4025 / 4029 / 4033 / 4035 / 4039 / 4041（怪物）
  和 3959 / 3968 / 3971（士兵）。3968 在 objects.res 里叫“Монстр1”，贴图却是拿加特林的重甲兵，所以归士兵。AS2R 游戏里没有这些对象，所以只有 `MapFormat::AS2C` / `AS2OE` 的工程才抽得到（`gore_corpse_vids()`）。
  AS2C 地图在文件格式上与 AS2R 完全相同，所以单独加了 Project Version `AS2C`（`io::MapFormat::AS2C`，writer 按 AS2R 写，逐字节相同）。
  AS2C 工程改成 AS2R 后，读项目不报错，`generate_map()` 遇到特有尸体时报错返回 false（`gore_vid_available()`），提示用橡皮擦掉。
- 一笔（`scatter_gore()`）：尸体卡用物资箱的 `fill_from_center()` 从圆心向外放尸体，只有同一笔里的尸体互相保持间距；
  **不避让地图上已有的任何东西**，尸体和血可以随便叠，同一处再点一下就再多一组（物资箱仍会避让已有箱子）；
  每具尸体在半轴 80 × 60 的椭圆里均匀撒血（官方 |dx| / |dy| 的 p90）。
  血滩卡只在半轴 `radius` × `0.75 radius` 的椭圆里撒血，同一处多点几下血会更多。
  参数只有笔刷半径、尸体间距（血滩卡隐藏）、血迹数（每具尸体或每笔，实际在 [n/2, 3n/2] 里随机）。
  默认：骨头堆 80 / 35 / 4，怪物和士兵 40 / 90 / 8（点一下基本就是一具），血滩 50 / – / 6（`default_gore_brush()`）。
- 血迹配方：尸体周围 110 45%、120 35%、916 13%、1319 7%；血滩 110 60%、120 30%、916 10%。
  怪物 / 士兵尸体周围的血 z 在 5–15 随机，盖在尸体上（z 0 的血会被尸体压住）；血滩的血同样 5–15；骨头堆的血 z 0。
  916（扩散血动画）z 固定 20。**不放 154**（“竖直”血迹，用户决定）。方向全部 0..255。
- army：1532 写 2（官方地图全部是 2），其余尸体和全部血迹写 0；scale 1、gamma 0。`build_gore()` 先输出血迹再输出尸体。
- 擦除（`erase_gore()`）：橡皮删圆内属于当前卡的 VID；血迹属于每张卡，所以在尸体卡上擦会连血一起擦，在血滩卡上只擦血。
- 编辑器：照物资箱。`Viewport::goreKind` 非空即 Gore 卡，与 `lootKind` 至多一个非空，同样不在 `resetInteraction()` 里清；
  拖动时每移动半个笔刷半径落一笔，落到地图外的丢掉。参数每张卡一份，只在本次会话里记住（`MainWindow::goreBrushes_`），不做预设。
- 笔刷虚影（物资箱和尸体血迹共用）：光标处画下一笔的虚影（不透明度同装饰物放置虚影 0.55）和绿色虚线圆（`SCATTER_BRUSH_GHOST_BORDER`，
  与装饰物虚影框同色）。每一笔都在 `RandomSeedScope(scatterSeed_)` 里跑核心散布函数（`rollLoot()` / `rollGore()`），
  所以虚影和点下去放的完全一样（含物资箱对已有箱子的避让、出地图丢弃）；`RandomSeedScope` 结束后恢复共享随机序列。
  右键（手绘模式下）换一个种子；每落一笔也自动换种子，拖动时每一堆都不同。拖动中不画虚影。
  核心散布函数不打日志（虚影每次鼠标移动都会调用），落笔日志由编辑器写。
- 画布：`drawGore()` 把所有尸体血迹交给 `drawMembers()`，画在物资箱下面。血迹和 1532 在 `placeholder_vids` 黑名单里，
  没有游戏目录时 `drawMembers()` 什么都不画，所以 `drawGore()` 给解不出贴图的血迹画暗红小圆点、给 1532 画叉号；装饰物 stamp 不受影响。
- 项目 JSON：顶层 `gore`（`[{vid, x, y, z, direction}]`）只在非空时写；未知 VID 或 AS1 工程出现 `gore` 直接报错。

## 测试与 Golden 文件

核心测试在 `tests/test_*.cpp`，编辑器测试在 `tests/editor/test_editor.cpp`（交互、旧 JSON 兼容、四种输出、中文路径、
多语言 key 集合、快捷键分发）；五种 Project Version（AS1 / AS2 / AS2R / AS2C / AS2OE）都要能生成。

重点测试：

- `tests/test_array_layout.cpp`
- `tests/test_incubator_builder.cpp`
- `tests/test_desk_builder.cpp`
- `tests/test_wall_builder.cpp`
- `tests/test_door_builder.cpp`
- `tests/test_map_writer.cpp`
- `tests/test_loot.cpp`
- `tests/test_gore.cpp`

Golden 文件在 `tests/golden/`。改动导致 golden 输出变化时，必须能解释是预期行为变化，不要静默更新。

当前测试路径工具集中在：

- `tests/utils/test_utils.h`

新增测试时，优先复用这里的路径辅助函数，不要把当前工作目录假设写死在测试里。

## 构建与运行

编辑器使用 MSVC + Qt 6.11.1（Qt Base、Qt Image Formats）。
只维护 `msvc-release` 一个预设；Qt SDK 路径在 `CMakePresets.json` 的 `CMAKE_PREFIX_PATH`。

CMake 分四个文件：根 `CMakeLists.txt` 只放全局设置、依赖（spdlog、gtest、Qt，以及 `src/3rdparty/simpleini/` 里随仓库提交的
SimpleIni v4.22 头文件，INTERFACE 库 `simpleini`，只有编辑器链接）和三个 `add_subdirectory`；
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

运行离线 Python 工具（`_prepare/`）前先启用虚拟环境：

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

- Stamp 先看 `decoration_library.*` 和库地图本身，测试用冻结副本 `tests/golden/decoration/01dorm_library.map`（AS2）
  和 `tests/golden/decoration/AS1/08leisure.map`（AS1，放子目录里，避免被 AS2 的 `load_directory` 测试读到）
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

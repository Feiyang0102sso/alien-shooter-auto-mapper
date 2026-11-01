# 临时讨论记录：散布笔刷、Loot Box、灯光、尸体

> 临时文档。**Loot Box 和尸体血迹（Gore）已实现**，规则已迁入 AGENTS.md“物资箱”“尸体血迹”和 CONTEXT.md“Loot Box”“Gore”；
> 本文件保留给还没做的灯光讨论，全部做完后删除。
> 数据来源：`E:\alien_shooter\Alien Shooter2 Conscription\Maps\Level_*.map`（AS2C）与 `Maps\!AS2R\Level_*.map`（AS2R），
> 用 `_prepare/AS2/as2_map_unpack.py` 解析；VID 名称读自 `objects.res`。

## 决策

- **优先级**：散布笔刷 + 掉落物先做，尸体血迹随后（都已完成）；灯光还没做。
- **新货架**：与 Wall Sets、Decoration Sets 平级的第三个货架，先放 Loot Box（木箱 / 金属箱 / 蛋），之后放灯光和尸体。
- **箱子、尸体不做成传统 stamp**：要像笔刷一样随机，但也不能像阵列那样整齐；箱子可以叠放。
- **掉落表是全局属性**：按 VID 绑定，整张地图上所有该 VID 的 sprite 都会带上，包括通过装饰物 stamp 放出来的成员。
- **掉落表支持多组预设**，预设写进 `AutoMapper.cfg`。
- **箱子只有两种**：木箱 1225、金属箱 2332。集装箱、直升机箱等不算 Loot Box。
- **箱子统计以 AS2R 安装目录的原版地图为准**（`E:\alien_shooter\AlienShooter2 Reloaded\Maps`，2008 年的文件）。
  AS2C 目录下的地图为了省事删过一大批箱子，箱子数据不可信；`!AS2R` 副本里也有 L01、L03、L06 和原版不一致。

## 一、高频手工内容总览（AS2R 18 张官方图，194,145 个 sprite）

| 类别 | 主要 VID | 数量 | 占比 |
|---|---|---|---|
| 血迹 | 120 / 154 / 110 / 916 / 918 | ~48,900 | 25% |
| 室外地面 | 505 / 512 / 510 / 1127 / 1711 | ~24,800 | 13% |
| 草、树、灌木、石头 | 506 / 513 / 1509 / 1526 / 1506 / 1504 | ~19,700 | 10% |
| 怪物 | 1025 / 1050 / 1840 / 1092 / 1037 / 1810 / 1830 | ~8,900 | 5% |
| 阴影 | 135 / 144 / 133 | ~8,600 | 4% |
| 线性物件（沙袋、护栏、道路） | 1220 / 1224 / 3619 / 1500 / 1501 / 1221 | ~7,700 | 4% |
| 尸体、碎片 | 1532 / 183 / 1328 / 1319 | ~8,700 | 4% |

## 二、掉落物（Loot）

### 存储格式

- 掉落写在 **SPRD 每个条目末尾的 nameText** 里，道具名之间用空格分隔，空槽写 `EMPTY`。详见 `_prepare/AS2/AS2_MAP.bt` 的 `SPRD_Sub`。
- SPRD 里另有一个 `DropList`（`itemCount + VID[]`），AS2C 不用它。全部数据里只在 `!ASR/Level_02` 发现 3 个 1225 用了它（1676、1640）。
- 官方编辑器写出的 nameText 有时以空格开头，例如 `' Ammo_shotgun'`。
- writer 目前 nameText 恒为空：`src/auto_mapper/io/as2_map_writer.cpp` 的 `build_as2_sprd_section()`。

### 道具名来源

- **决策：程序不读取、不校验 `Weapon.cfg`。** 掉落内容只由用户在文本框里输入的道具名决定，原样写进 nameText。
- 参考信息：道具名定义在游戏目录的 `Weapon.cfg` 里，AS2R 与 AS2C 相同，共 177 个
  （`Ammo=` 11、`Weapon=` 73、`Armor=` 17、`Equip=` 41、`Implant=` 17、`Objects=` 18）。

### 官方用法（AS2R 原版，32 张图）

| VID | 名称 | 数量 | 带 nameText | army | noDir |
|---|---|---|---|---|---|
| 1225 | 木箱 | 772 | 620（80%） | 2 | 9 |
| 2332 | 金属箱（基地箱 2） | 189 | 182（96%） | 2 | 10 |
| 2750 | 异形蛋 | 1,200 | 1,200，但全是 `m1` | 2 | 4 |
| 2751 | 破损的蛋 | 52 | 0 | 0 | 4 |
| 2357 / 442 / 1269 / 2318 / 1282 / 1281 | 集装箱、直升机箱、弹药箱等 | 339 / 213 / 202 / 66 / 61 / 45 | **0** | **0** | 40 / 5 / 4 / … |

- 能装掉落的箱子 army 都是 2，陪衬容器 army 都是 0。
  `vid_armies_as2.h` 里没有 1225 / 2332 / 2750，笔刷放的箱子要自己写 army 2。
- 两种箱子在地图里**从不混用**：L01–L07、L12–L15 只用木箱，L09–L11 只用金属箱，只有 L08 两种都有。

**掉落机制**：nameText 是一个候选池，**游戏从整条文本里随机挑一个**。
所以官方文本条里会重复写同一个道具来提高它的权重，`EMPTY` 则表示“什么都不掉”的那部分概率（这两点是从数据推断的）。

**官方最常用的整条文本**（AS2R 原版）
- 1225，共 772 个，66 种不同的文本条：
  - 152 个：空（不写 nameText）
  - 118 个：`EMPTY EMPTY Ammo_shotgun Ammo_minigun Ammo_minigun Ammo_rocket Ammo_energy Obj_money3`
  - 48 个：`Obj_money1`
  - 35 个：`Obj_money2`
  - 35 个：`EMPTY EMPTY Ammo_shotgun Ammo_minigun Ammo_rocket Ammo_minigun Obj_money3`
  - 23 个：`EMPTY Ammo_shotgun Ammo_minigun Ammo_rocket EMPTY Equip_medikit_small Obj_money3`
  - 19 个：`EMPTY Ammo_shotgun Ammo_minigun Ammo_energy`
  - 19 个：`Ammo_minigun Ammo_rocket Ammo_energy Obj_money3`
  - 18 个：`Ammo_shotgun`；13 个：`Bonus_health50`
- 2332，共 189 个，14 种不同的文本条：
  - 76 个：`EMPTY EMPTY Ammo_shotgun Ammo_minigun Ammo_minigun Ammo_rocket Obj_money3 Ammo_energy`
  - 46 个：`Ammo_minigun Ammo_rocket Ammo_ballon Ammo_energy Obj_money3`
  - 23 个：`EMPTY EMPTY Ammo_shotgun Ammo_minigun Ammo_minigun Ammo_rocket Ammo_energy Obj_money3`
  - 15 个：`Bonus_health50`；10 个：`Ammo_shotgun Ammo_minigun Ammo_rocket`；7 个：空
- 每张图通常混用 3–20 种文本条，所以“每箱从多条文本里按概率选一条”符合官方做法。

**蛋**
- AS2C：1,881 / 1,904 个蛋的掉落几乎固定为 `Ammo_shotgun Ammo_minigun Ammo_rocket Bonus_health10` + 10 个 `EMPTY`（共 14 槽）。
- AS2R：1,200 个蛋的 nameText 全是 `m1`。**`m1` 是给 quest 用的特殊任务标记**，不是掉落。
  所以 nameText 不只用来写掉落，也可能是任务标记；掉落表只写笔刷和 stamp 生成的 sprite，不会碰到手工写的任务标记。

## 三、箱子与蛋的摆放规律

### 箱子（AS2R 原版，1225 + 2332 共 961 个）

- **成堆**：共 323 堆（相距 90 内算同一堆）。单箱 101 堆（31%），2 箱 69，3 箱 40，4 箱 47，5 箱 24，6 箱 17，7 箱 17，8 箱以上 8。
- **不混搭**：没有一堆同时含有 1225 和 2332。
- **间距**：和最近箱子的距离，p10 / p25 / p50 / p75 / p90 = 32 / 38 / 44 / 53 / 91。
- **箱子叠箱子很少**：只有 13 个（1.4%）。叠一层的 dz 约 20–30（1225 约 25，2332 约 30），水平偏移在 30 以内。
- **另有 111 个箱子被抬高**，z 为 90、120、180 等，是放在其他物件上，多半是 90 高的直升机箱 442 或货架。
- **方向**：按 `noDir` 吸附。1225 是 9 方向（官方取值集中在 0 / 28 / 56 / 85 / 113 / 142 / 170 / 199 / 227），
  2332 是 10 方向（0 / 25 / 51 / 76 / 102 / 128 / 153 / 179）。
- scale 全为 1，gamma 全为 0。
- 旧的 AS2C 统计（2,258 个箱子、20% 叠放、13% 混搭）混进了陪衬容器，AS2C 地图本身也删过箱子，作废。

### 蛋（2750 活蛋 / 2751 破损蛋）

- **铺成密集的一片**，不成堆：和最近蛋的距离中位数 32–40。
- scale 全部为 1，z 基本为 0。
- 破损比例因关卡而异：AS2C L07 约 30%（297 / 999），L08 和 AS2R L14 为 0%。

## 五、灯光

### VID 说明

- **228** `FX:Sprite Ligth Glow 平滑照明`：最常用的光圈（1,315 个），有 23 种 scale、45 种 gamma。
- **223 / 224 / 1370**：其他光圈（441 / 318 / 326 个）。
- **1317** `LIGHTS:白色光斑 (Shadow)`：地面光斑，225 个，scale 1–2，gamma 为 0 或 -100。
- **1306** `FX:雾团`：1,448 个，scale 多为 3（691 个）、4、5，z 为 0。适合做散布笔刷的一个预设。
- **220** 是 `hide:弹片烟雾`，不是灯。
- 地图环境光存在 GRPH 里（UI 值多在 30–120，偏暗）。项目目前用固定模板，**还不能设置环境光**。

### 伴生光圈（物件 → 光圈，80 范围内最近的物件）

| 物件 | 光圈 | 次数 | 典型 scale | 典型 gamma |
|---|---|---|---|---|
| 2711 岩浆裂缝 | 228 | 396 | 0.25 | (-180,-180,-255) |
| 2750 异形蛋 | 228 / 224 | 102 / 45 | 0.25 / 0.5 | (-180,-180,-255) |
| 2333 化学桶 | 228 | 92 | 0.3–0.4 | (-240,-150,-240) |
| 1297 商店电脑 | 223 | 64 | 0.25 | (-180,-180,-100) |
| 2710 岩浆池 | 224 | 49 | 1.0 | (-170,-210,-255) |
| 1321 蒸汽发生器 | 228 / 224 | 21 / 14 | 0.3 / 0.5 | — |
| 2322 手术床 | 228 | 31 | 0.2 | (-200,-200,-180) |
| 2536 信息块 | 223 | 27 | 1.0 | (-200,-100,-200) |

- **现有 AS2 装饰物库地图共 17,744 个 sprite，没有一个光圈。**
- 方案：在核心里放一张“伴生光圈”表（形式类似 `vid_armies_as2.h`），生成时对所有输出 sprite（含 stamp 成员）自动补光圈，用勾选项控制开关。

### 墙灯

- 1226 / 1138 / 1258 / 1227 等墙灯大多没有配光圈（1226 只有 24 / 370），灯本身就是光源。
- 方向跟着墙的轴向走。粗略统计：1226 挂在 `/` 墙上取 128，挂在 `\` 墙上取 149。这是按“最近的墙”判断的，有噪声，需要标定。
- 方案：做成“墙面挂件阵列”，复用 Straight Wall Run 与稀有墙件的选位逻辑。墙灯、墙面 logo（1308）都适用。

### 预览

- 官方编辑器截图显示：环境光把整个场景压暗，光圈把地面提亮成彩色。
- 方案：离屏图层先铺环境色，光圈经 `game_assets` 解码、套上 gamma 和 scale 后用 `CompositionMode_Plus` 叠加，最后用 Multiply 盖到画布上。
- 画布不画地板和墙的贴图，所以只能看到“平底色上的光照分布”。

## 六、Loot Box 货架草案

### 卡片

| 卡片 | VID | 掉落表 |
|---|---|---|
| 木箱 | 1225 | 木箱表 |
| 金属箱 | 2332 | 金属箱表 |
| 蛋 | 2750（2751 破损蛋按概率混入） | 蛋表；破损蛋不写 |

笔刷放出的箱子和蛋固定写 army 2，破损蛋写 army 0。

### 笔刷参数（右侧属性面板）

通用：
- 笔刷半径
- 素材勾选与权重：本卡片里的各个 VID
- 随机方向：按 `noDir` 吸附
- 避让：墙、门、装饰物 stamp 框

木箱 / 金属箱：
- 堆间距：两堆中心的最小距离（默认值待标定）
- 每堆箱数：min–max，默认 1–7，分布参照官方（31% 单箱，其余集中在 2–4）
- 堆内间距：默认 38–53，即官方的 p25–p75
- 叠放概率：官方只有 1.4%，默认 5% 左右即可；最大层数 2
- 叠放高度：1225 约 25，2332 约 30，水平偏移 30 以内；放在内部常量里，不暴露在 UI 上

蛋：
- 间距：默认 35，铺满笔刷范围
- 破损概率：默认 0%，官方范围 0–30%
- 破损蛋不写掉落（固定规则）

### 掉落表（全局，独立对话框）

- 每张表绑定一组 VID（木箱 / 金属箱 / 蛋），生成时写入整张图所有这些 VID 的 nameText，包括 stamp 成员。
- 每行：左边是**一整条文本**（自由输入，例如 `EMPTY EMPTY Ammo_shotgun Ammo_minigun Obj_money3`），右边是概率 %。
  程序不解析、不校验，原样写出，也不限制长度。
  UI：一行显示不下时文本框自动变宽（或换行增高），保证整条文本始终完整可见。
- **工具只负责挑哪一整条**：每个箱子按概率独立选一行，把这一整条文本写进 nameText。
  文本条里挑哪个道具由游戏决定。
- 各行概率之和 ≤ 100%，剩余部分显示为“不写”（只读），抽中它的箱子 nameText 留空。
- 默认预设可以直接取上面统计到的官方常用文本条和出现比例，以及 AS2C 蛋的
  `Ammo_shotgun Ammo_minigun Ammo_rocket Bonus_health10` + 10 个 `EMPTY`。
- 预设：保存、载入、删除；写进 `AutoMapper.cfg`。项目 JSON 另外保存当前使用的表，保证换机器后打开项目结果一致。
- 只支持 AS2 系列，AS1 的 SPRD 格式另议。

## 七、待确认

1. 给 AS2R 的蛋写道具名会不会生效？（`m1` 已确认是 quest 标记）
2. ~~掉落语义~~：已定。每箱按概率选一整条文本写入，游戏从文本条里随机挑道具。
3. 一条文本的长度上限：引擎上限未知，文件格式本身不限（SPRD 子块长度是 uint32）。
   - 官方原版最长：AS2C `Level_08` 的蛋，14 个词、114 字节（含开头空格和结尾 `\0`）。
   - 全部地图最长：自制 `!mymod/Level_08` 的蛋，14 个词、119 字节。
   - 14 个词可能是官方编辑器的槽位数，不一定是引擎上限。工具不设上限。
4. 掉落在每次生成时重新抽取（核心的 `Random` 用 `random_device` 播种，没有固定种子），还是在项目里存种子保证可复现？
5. 笔刷结果的存储：建议存成具体的一个个实例（VID、坐标、z、方向），可以用橡皮擦掉；掉落不存在实例上，而在生成时按全局表写入。

## 八、实现方案：Loot Box 笔刷 + 掉落表（已实现）

> 实现时的修正：箱子方向改为直接用官方地图出现过的值（木箱 9 个、金属箱 8 个）。2332 的 objects.res 写着 10 方向，但 204 / 230 没有帧；而且这些方向其实是不同款式的箱子，不是旋转。掉落表对话框按当前卡片打开，不做三个页签；代码里不内置预设；预设后来从 cfg 移到 exe 旁的 `loot_presets.ini`（SimpleIni，不分卡片、按节名区分；官方常用文本条见第二节）。再之后加了模板，放在仓库根目录 `loot_presets.ini`（构建和打包时放到 exe 旁）：初期 / 中期 / 后期按 AS2R 关卡分段统计，只留钞票（无金条）、弹药、血量拾取物，外加异形蛋一节。之后又改为一笔一堆、从中心向外铺，默认叠放 15%、破损 10%。

### 范围

- 只做 AS2 系列。AS1 工程里不显示 Loot 货架；项目 JSON 里出现 Loot 数据时，AS1 工程直接报错（和曲线墙一样）。
- 只做 Loot Box：木箱 1225、金属箱 2332、蛋 2750（破损蛋 2751）。灯光、尸体以后在同一个货架框架上加。
- 第一版不做：单个箱子的选中和拖动（用橡皮擦掉重刷）、避让墙体、撤销（编辑器目前本来就没有撤销）。

### 数据流

```text
编辑器笔刷 ──调用核心 scatter_loot_boxes()──> 具体实例 LootBox（存进项目 JSON）
项目 JSON ──Project::generate()──> MapRequest{ loot_boxes, loot_tables }
generate_map(): 墙 → 曲线墙 → 门 → 装饰物 → Loot Box → 【掉落表统一写 nameText】 → writer
```

- 摆放规则和随机算法放在核心里，编辑器只负责交互，这和现有的“核心与编辑器的边界”一致，也方便单测。
- 掉落表在生成的最后一步统一处理：按 VID 匹配全部输出 sprite，所以笔刷放的箱子和 stamp 里的箱子走同一条路径。
  现有 AS2 库地图里已有 245 个木箱、2 个金属箱（03storage 132 个、07pipeline 42 个、21barricade 34 个……），它们都会带上掉落。

### 1. 核心：writer 支持 nameText

- `io::Sprite` 增加 `std::string name_text`，默认为空。
- `build_as2_sprd_section()`：subSize 由固定 17 改为 `16 + name_text.size() + 1`，原样写入文本加 `\0`。
- name_text 为空时输出逐字节不变，所以现有 golden 不应变化（要用测试确认）。
- AS1 writer 不写 nameText；sprite 带了 name_text 时打 warning。
- 测试：`test_map_writer.cpp` 加一个用例，写入后用 SPRD 布局读回 subSize 和文本。

### 2. 核心：掉落表

新模块 `core/loot/loot_table.*`：

```cpp
struct LootLine {
    std::string text;   // 整条文本，原样写进 nameText
    double percent = 0; // 0..100
};

struct LootTable {
    std::vector<int32_t> vids;   // 绑定的 VID，例如木箱表 = {1225}
    std::vector<LootLine> lines; // 概率之和 <= 100，剩余部分不写
};

// 对每个 vid 命中的 sprite 独立抽一行，写入 name_text；没抽中的保持为空。
void assign_loot_text(std::vector<io::Sprite>& sprites, const std::vector<LootTable>& tables);
```

- `MapRequest` 增加 `std::vector<LootTable> loot_tables`。
- 概率之和超过 100 时 `generate_map()` 报错并返回 false（UI 会先拦住，这里是兜底）。
- 测试：100% 的单行表让所有命中的 sprite 得到同一条文本；0 行的表不写；stamp 成员也会被写；破损蛋 2751 不受影响。

### 3. 核心：Loot Box 摆放

新模块 `core/loot/loot_box_builder.*`，常量集中写在文件开头：

| 常量 | 值 | 来源 |
|---|---|---|
| 木箱 / 金属箱 / 蛋 / 破损蛋 VID | 1225 / 2332 / 2750 / 2751 | — |
| 方向数 | 9 / 10 / 4 / 4 | objects.res `noDir` |
| army | 箱子和活蛋为 2，破损蛋为 0 | AS2R 原版 |
| 叠放高度 | 木箱 25，金属箱 30 | AS2R 原版 dz |
| 叠放水平偏移上限 | 30 | AS2R 原版 |

```cpp
enum class LootBoxKind { Wood, Metal, Egg };

struct LootBox {            // 一个具体实例，Map Coordinates
    int32_t vid = 0;
    float x = 0, y = 0, z = 0;
    uint32_t direction = 0;
};

struct LootBrushSettings {
    LootBoxKind kind = LootBoxKind::Wood;
    float radius = 150;              // 笔刷半径
    float pile_spacing = 160;        // 堆与堆中心的最小距离
    int pile_size_min = 1;           // 每堆箱数
    int pile_size_max = 4;
    float box_spacing_min = 38;      // 堆内相邻箱子的距离（官方 p25–p75）
    float box_spacing_max = 53;
    double stack_chance = 0.05;      // 叠放概率
    float egg_spacing = 35;          // 蛋的间距
    double egg_broken_chance = 0.0;  // 破损概率
};

// 在 center 周围一次落笔，返回新增实例；existing 用于保持间距，不会和已有实例挤在一起。
std::vector<LootBox> scatter_loot_boxes(const LootBrushSettings& settings, float center_x, float center_y,
                                        const std::vector<LootBox>& existing);

// 实例 -> sprite：补上 army、scale 1、gamma 0。
std::vector<io::Sprite> build_loot_boxes(const std::vector<LootBox>& boxes);
```

**箱子算法**（一次落笔）：
1. 在笔刷圆内随机取若干候选点，离已有箱子都至少 `pile_spacing` 的才成为新堆的中心（Poisson-disk 拒绝采样）。
2. 每堆箱数在 `[min, max]` 里均匀抽取。第一个箱子放在中心；之后每个箱子挑堆里一个已放的箱子，
   在 `[box_spacing_min, box_spacing_max]` 的距离、随机角度处放下，离堆内其他箱子太近就重试，重试次数用完就放弃这一个。
3. 每个后续箱子有 `stack_chance` 的概率改为叠到堆里一个地面箱子上：z = 叠放高度，水平偏移 ≤ 30。最多叠一层。
4. 方向从 `floor(k * 256 / noDir)` 里随机取，正好对上官方取值（1225：0 / 28 / 56 / 85 / …）。

**蛋算法**：笔刷圆内按 `egg_spacing` 做 Poisson-disk 铺满；每个蛋有 `egg_broken_chance` 的概率换成 2751；方向取 0..255（官方 253 种）。

测试：堆间距与堆内间距下限、叠放的 z 只会是 0 或叠放高度、方向都在 `noDir` 取值里、破损概率为 0 / 1 的两个极端、笔刷落在已有箱子上时不再加箱子。

### 4. 编辑器：项目数据

- `Project` 增加 `std::vector<LootBox> lootBoxes` 和 `std::vector<LootTable> lootTables`（固定三张：木箱、金属箱、蛋）。
- JSON（只在非空时写，旧版本读取时会忽略）：

```json
"loot_boxes": [{"vid": 1225, "x": 1200, "y": 800, "z": 0, "direction": 28}],
"loot_tables": {
  "wood":  [{"text": "EMPTY EMPTY Ammo_shotgun Ammo_minigun Obj_money3", "percent": 15.3}],
  "metal": [],
  "egg":   []
}
```

- `Project::generate()` 把两者传进 `MapRequest`。
- 测试：`legacyJsonRoundTrip` 增加 Loot 数据；`legacyFixturesKeepGeneratedContent` 必须逐字节不变。

### 5. 编辑器：第三个货架

- `MainWindow::decorationShelf_`（bool）改为 `enum class ShelfMode { Walls, Decorations, Loot }`，
  `chooseShelf()`、`rebuildShelf()`、`chooseSet()`、`chooseItem()`、`activateMode()`、`syncMode()` 按三种模式分支。
- `LeftShelfPanel` 增加第三个按钮；AS1 工程隐藏它。
- 三张卡片：木箱、金属箱、蛋。卡片预览在有游戏目录时用 `StampSprites` 渲染一个样例，没有时显示“未配置”框（和装饰物卡片一致）。
- 工具：Loot 货架上只显示“手绘（笔刷）”和“橡皮”，隐藏 Select 和网格工具。
  - 仿照曲线墙卡：`Viewport` 增加 `lootKind`（-1 表示不在 Loot 卡上），`DrawingMode::Freehand` 在 Loot 卡上就是笔刷，不新增模式和快捷键。
  - `resetInteraction()` 不清 `lootKind`，`setWall()` / `armStamp()` / `chooseShelf()` 负责清掉，避免在笔刷和橡皮之间切换时丢卡。

### 6. 编辑器：画布

- **笔刷**：按下时落一笔；拖动时光标每移动半个笔刷半径再落一笔。每笔调用 `scatter_loot_boxes()`，结果追加进 `project.lootBoxes`。
- **橡皮**：在 Loot 卡上删除橡皮圆内的 Loot Box，只删当前卡片对应的那一种。
- **预览**：鼠标处画笔刷圆（虚线，和橡皮预览圆同一画法）。
- **绘制**：有游戏目录时，用 `StampSprites::sprite()` 按 `(x, y - z)` 画真实贴图，按 y 排序，叠放的箱子画在下层箱子之后；
  没有游戏目录时画小标记（箱子画方块、蛋画圆点），颜色加到 `ui/colors.h`。
- 和装饰物一样受“贴图不透明度”设置影响。

### 7. 编辑器：属性面板

Loot 货架时，右侧显示当前卡片的笔刷参数：

| 参数 | 木箱 / 金属箱 | 蛋 |
|---|---|---|
| 笔刷半径 | ✓ | ✓ |
| 堆间距 | ✓ | — |
| 每堆箱数 min–max | ✓ | — |
| 箱子间距 min–max | ✓ | — |
| 叠放概率 | ✓ | — |
| 蛋间距 | — | ✓ |
| 破损概率 | — | ✓ |

- 下方一个“掉落表…”按钮，打开掉落表对话框。
- 参数只在本次会话内记住，每张卡片各自一份（第一版不写 cfg）。

### 8. 编辑器：掉落表对话框

- 三个页签：木箱、金属箱、蛋。每页是一个列表，每行：
  - 左：整条文本。单行输入框，宽度随内容增长；对话框随之变宽，超出屏幕后出现横向滚动条。
  - 右：概率 %（0–100，一位小数）。
  - 行尾删除按钮；列表下方“添加一行”。
- 底部只读显示“不写：x%”。概率之和超过 100% 时标红，并禁用“确定”。
- 预设下拉框：载入、另存为、删除。确定后写回 `project.lootTables`，并标记项目已修改。

**预设写进 cfg**：一行一条，同一个 key 可以重复出现，文本放在最后一个字段，所以里面可以有空格：

```text
# loot_preset = <kind>|<preset name>|<percent>|<whole text>
loot_preset = wood|AS2R 木箱|15.3|EMPTY EMPTY Ammo_shotgun Ammo_minigun Ammo_minigun Ammo_rocket Ammo_energy Obj_money3
loot_preset = wood|AS2R 木箱|6.2|Obj_money1
loot_preset = egg|AS2C 蛋|100|Ammo_shotgun Ammo_minigun Ammo_rocket Bonus_health10 EMPTY EMPTY EMPTY EMPTY EMPTY EMPTY EMPTY EMPTY EMPTY EMPTY
```

- `config.*` 增加读写“同一 key 的多行”的两个函数；现有单值 key 的读写不变。
- cfg 里还没有任何 `loot_preset` 时，写入内置的默认预设：AS2R 木箱、AS2R 金属箱（取官方常用文本条及其占比）和 AS2C 蛋。

### 9. 文案与文档

- 新文案两份 locale 一起加：货架按钮、三张卡片的标题和描述、笔刷参数、掉落表对话框。
- 完成后把“Loot Box / 掉落表 / 掉落文本条”写进 CONTEXT.md 词汇表，在 AGENTS.md 加一节，然后删掉本临时文档。

### 实施顺序

1. writer 的 nameText（独立、可单测、确认 golden 不变）
2. 掉落表 + `generate_map()` 接入
3. Loot Box 摆放算法
4. 项目数据与 JSON
5. 第三个货架 + 画布笔刷 / 橡皮 / 绘制
6. 属性面板参数
7. 掉落表对话框 + cfg 预设
8. 文案、文档收尾

前 3 步只动核心，每步都能单独跑测试；第 4–8 步是编辑器。

### 需要审核的决定（Loot Box，已定）

1. **笔刷手感**：按住拖动、沿路径连续落笔（类似画笔），而不是先圈一个区域再一次性填满。
2. **掉落每次导出都重新抽**：同一个项目每次导出，箱子里的文本条会不同；箱子的位置和方向是固定的。
3. **笔刷参数不写 cfg**，只在本次会话内记住；掉落表预设写 cfg，当前掉落表存在项目里。
4. **第一版不避让墙体**：箱子可能画进墙里，由用户自己控制落笔位置。
5. **默认值**：每堆 1–4 个、箱子间距 38–53、叠放 5%、蛋间距 35、破损 0%；堆间距 160 是估计值，需要试用后再调。

## 统计脚本（临时，在 scratchpad 中）

`C:\Users\Feiyang\AppData\Local\Temp\claude\E--coding-projects-c-projects-auto-mapper\bcaf2b65-bd1f-40ad-be49-a78e0251c914\scratchpad\`
下的 `box_r.py`（AS2R 原版箱子统计，用法：`box_r.py <scratchpad> <Maps 目录>`）、`stats.py`、`spatial.py`、`drops.py`、`names_drop.py`、`boxpile.py`、`eggs.py`、`blood.py`、`lightpair.py`、`walllamp.py`、`boxlight.py`；`names.py` 从 `objects.res` 导出 VID 名称表。

尸体血迹统计（第九节）的脚本在
`C:\Users\Feiyang\AppData\Local\Temp\claude\E--coding-projects-c-projects-auto-mapper--claude-worktrees-corpse-blood-brush-2006ad\ebc97523-4f73-4359-94a0-bb827b38f34f\scratchpad\`：
`names.py`（`names.py <游戏目录> <输出 json>`）、`gore.py`（`gore.py <scratchpad> R|C`）、`gore2.py`（孤立尸体）、`extras.py`（AS2C 特有尸体出现在哪）。

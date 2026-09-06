# Auto Mapper v1.0

🌍 **Language / 语言**

- [🇺🇸 English Version](#english-version)
- [🇨🇳 中文版](#chinese-version)

---

<a id="english-version"></a>
## 🇺🇸 English Version

Auto Mapper is an Alien Shooter map layout generator. It aim to help build a map quickly, then verify and refine
the result in a map editor.

### features

- Automatic placement of walls, doors, floors, and ceilings.
- Decoration presets for quickly placing common indoor decorations.
- Editor preview and project save/load support.
- Support for AS1 and the AS2 series (AS2, AS2R/C, and AS2OE) map formats and wall sets.

### Important warning

> [!WARNING]
> Auto Mapper's algorithms cannot be guaranteed to be perfect in every map or
> layout. Always treat the generated result as a starting point and verify it
> in your map editor. The map editor is the final authority.

Decoration previews have an additional limitation: the images shown in the UI
are for reference only and may not precisely represent the final in-game
appearance or contents. Use the map editor to confirm every decoration.

### Recommended workflow

Auto Mapper currently cannot edit an existing map. The recommended workflow is:

1. Create or prepare the map in Auto Mapper.
2. Finish and verify all automatic parts first: walls, doors, floors, and
   ceilings.
3. Open the generated map in your map editor and confirm that the automatic
   layout is correct before starting extensive manual editing.
4. For decorations especially, adjust a small part at a time and verify it in
   the map editor as you go.

### Changelog

- **v1.0（2026-09-06）:**
  - Added the ceiling generation algorithm.
  - Added decoration presets.
  - Added keyboard shortcuts.
  - Added support for the three AS2 variants: AS2, AS2R/C, and AS2OE.
- **v0.1.0 (2026-08-11):**
  - Initial pre-release.

---

<a id="chinese-version"></a>
## 🇨🇳 中文版

Auto Mapper 是一款 Alien Shooter 地图自动铺设工具。他旨在帮助你快速完成地图布局，再交由地图编辑器进行
校验和细化。

### 功能

- 自动铺设墙壁、门、地板和天花板。
- 提供常见室内装饰物的装饰预设，支持快速放置。
- 提供编辑器预览以及项目保存和加载功能。
- 支持 AS1 以及 AS2 系列（AS2、AS2R/C、AS2OE）的地图格式和墙体组。

### 重要警告

> [!WARNING]
> Auto Mapper 的算法不能保证在所有地图和布局下都完美无瑕。生成结果
> 始终只应作为起点，必须在地图编辑器中进行校验；最终以地图编辑器为准。

装饰物还有额外限制：UI 中的预览图片仅供参考，不能保证与游戏中的最终
外观或具体内容完全精准。一切装饰物结果仍应以地图编辑器中的实际显示为准。

### 推荐工作流

鉴于目前 Auto Mapper 还不具备编辑现有地图的能力，推荐按以下流程使用：

1. 先在 Auto Mapper 中创建或准备地图。
2. 优先完成并确认自动铺设部分没有问题，包括墙壁、门、地板和天花板。
3. 将生成的地图交给地图编辑器，确认自动布局正确后，再开始进一步的手动编辑。
4. 尤其是装饰部分，推荐每次调整一小部分，并在调整过程中持续回到地图编辑器验证。

### 更新日志

- **v1.0 （2026-09-06）：**
  - 新增天花板算法。
  - 新增装饰预设。
  - 新增快捷键。
  - 支持 AS2 的三个变体：AS2、AS2R/C、AS2OE。
- **v0.1.0（2026-08-11）：**
  - 首个预发布版本。

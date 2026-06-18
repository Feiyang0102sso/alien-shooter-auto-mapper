# Auto Mapper 项目规范与结构规划

## 技术栈规范
- **算法层**：必须优先使用 **C++** 编写（包括核心逻辑、线段处理、坐标换算、网格判定等）。除非遇到极度麻烦且影响效率的特例情况，才考虑降级使用 Python 处理算法逻辑。
- **UI与胶水层**：使用 Python 编写。主要负责实现前端交互（画板等）、调用 C++ 编译出的核心库，以及进行一些非核心的脚本化操作。

## 目录结构规划
为了适应后续的 C++ 与 Python 混编，以及将之前在 `.research` 里的实验代码迁移并模块化，项目将按照以下结构进行组织：

```text
auto_mapper/
├── docs/                      # 存放所有项目文档 (由 _pre_docs 演进)
│   ├── automapper.md          # 核心架构与设计决策文档
│   └── project_docs.md        # 当前规范与结构规划文档 (即本文档)
│
├── src/                       # 源码目录
│   ├── auto_mapper/           # C++ 核心命名空间与主文件夹
│   │   ├── common/            # 公共基础组件层 (配置、日志、版本)
│   │   │   ├── config.h/cpp   # 等距映射项目的全局配置解析
│   │   │   ├── logger.h/cpp   # 零依赖 C++20 格式化日志系统
│   │   │   └── version.h      # 版本常量定义
│   │   │
│   │   ├── core/              # 核心算法与数据处理层 (待建)
│   │   │   ├── geometry.cpp/h # 等轴测坐标转换、网格吸附、居中偏置计算
│   │   │   └── wall_builder.cpp/h # 接收矢量线段，判定立柱/直墙，输出 Sprite 列表
│   │   │
│   │   └── io/                # 二进制文件读写层 (待建)
│   │       ├── map_writer.cpp/h   # 读取模板，拼装 SPR/SPRD 段，生成最终 .map
│   │       └── templates/         # 存放 empty.map 空白模板文件
│   │
│   └── python_layer/          # Python 胶水层与 UI 层
│       ├── main_ui.py         # 前端画板及用户交互主入口
│       └── map_unpacker.py    # 迁移自 _pre-demo，用于解包和验证 .map
│
├── tests/                     # 测试用例目录 (迁移自 _pre-demo)
│   ├── test_wall_algorithm.py # 测试算法生成结果是否符合预期
│   └── test_binary_write.py   # 测试生成的二进制结构是否合规
│
├── scripts/                   # 辅助工具和开发脚本
│   └── AS1_MAP.bt             # 010 Editor 的二进制解析模板
│
├── .venv/                     # Python 虚拟环境
├── requirements.txt           # Python 依赖清单
├── CMakeLists.txt             # (预留) C++ 构建脚本
└── README.md                  # 项目简介与快速启动指南
```

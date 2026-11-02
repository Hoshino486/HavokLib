
<h1 align="center">Havok 工具集</h1>

<p align="center">
    Havok 工具集是 HavokLib 下的一组转换工具。
</p>

<p align="center">
    本工具集基于 Spike 框架运行。
</p>

<p align="center">
    想了解如何高效使用，请看
    <b><a href="https://github.com/PredatorCZ/Spike/wiki/Spike">Wiki</a></b>。
</p>

<p align="center">
<b><a href="https://github.com/PredatorCZ/RevilLib/releases">最新发行版</a></b>
</p><h2>模块列表</h2>
<ul>
<li><a href="#Havok2GLTF">Havok2GLTF</a></li>
</ul>

## Havok2GLTF

### 模块命令: hk_to_gltf

把 Havok 数据转换为 GLTF。

支持动画与骨骼数据。

### 输入文件规则: `.hkx$`、`.hka$` 或用户指定

### 设置

- **extension-patterns**

  **命令行长选项:** ***--extension-patterns***\
  **命令行短选项:** ***-p***

  以逗号分隔指定用于识别文件的扩展名规则。

- **animation**

  - **blend-override**

    **命令行长选项:** ***--blend-override***\
    **命令行短选项:** ***-b***

    **默认值:** AUTO

    **可选值:** AUTO, NORMAL, ADDITIVE_DEPRECATED, ADDITIVE

    覆盖混合提示。

  - **sample-rate**

    **命令行长选项:** ***--sample-rate***\
    **命令行短选项:** ***-r***

    **默认值:** 60

    指定 GLTF 的每秒帧数。

  - **scale-type**

    **命令行长选项:** ***--scale-type***\
    **命令行短选项:** ***-S***

    **默认值:** NONE

    **可选值:**

    - NONE: 忽略缩放轨道。

    - INDEPENDENT:
    输入的缩放轨道不影响子节点位置，
    也不会像 MATRICIAL 模式那样引入切变。

    选择节点缩放轨道的处理模式。

  - **filename-anims**

    **命令行长选项:** ***--filename-anims***\
    **命令行短选项:** ***-f***

    **默认值:** false

    对未命名动画使用文件名代替 Motion[]

- **skeleton**

  - **skeleton-path**

    **命令行长选项:** ***--skeleton-path***\
    **命令行短选项:** ***-s***

    指定骨骼 hkx 文件的路径。

  - **generation**

    **命令行长选项:** ***--generation***\
    **命令行短选项:** ***-G***

    **默认值:** DEFAULT

    **可选值:**

    - DEFAULT: 不对骨骼做任何修改。

    - AUTO: 自动检测带子节点、可能出现缩放动画的节点。

    - MANUAL: 在 gen-cbones 中手动指定出现缩放运动且带子节点的骨骼。

    设置额外的骨骼修改。

  - **gen-cbones**

    **命令行长选项:** ***--gen-cbones***\
    **命令行短选项:** ***-B***

    以逗号分隔的骨骼名称，缩放轨道将应用到这些骨骼。

  - **visualize**

    **命令行长选项:** ***--visualize***\
    **命令行短选项:** ***-V***

    **默认值:** true

    为骨骼创建可视化网格。（在 Blender 中强制生成骨架对象）

  - **create-root-node**

    **命令行长选项:** ***--create-root-node***\
    **命令行短选项:** ***-N***

    **默认值:** false

    按 `root-node-name` 设置强制创建根节点。当骨骼内部存在多个根节点时会自动生成该节点。（用于分离根运动）

  - **root-node-name**

    **命令行长选项:** ***--root-node-name***\
    **命令行短选项:** ***-A***

    **默认值:** {SKELETON_NAME}

    启用 create-root-node 时，按指定模式生成根节点名称。
`{SKELETON_NAME}` 会被替换为实际的骨骼名称。例如 `root_{SKELETON_NAME}_bone` 会生成 `root_Refernce_bone`（骨骼名称为 Reference）

- **scene**

  - **units**

    **命令行长选项:** ***--units***\
    **命令行短选项:** ***-u***

    **默认值:** METER

    **可选值:** MM, CM, DM, METER, KM, INCH, FEET, MILE, YARD, CUSTOM

    输入场景的单位。

  - **custom-scale**

    **命令行长选项:** ***--custom-scale***\
    **命令行短选项:** ***-C***

    **默认值:** 1

    使用 CUSTOM 单位时设置单位缩放。

  - **up-axis**

    **命令行长选项:** ***--up-axis***\
    **命令行短选项:** ***-U***

    **默认值:** Y+

    **可选值:** X-, Y-, Z-, X+, Y+, Z+

    输入场景的上轴。

  - **forward-axis**

    **命令行长选项:** ***--forward-axis***\
    **命令行短选项:** ***-F***

    **默认值:** Z+

    **可选值:** X-, Y-, Z-, X+, Y+, Z+

    输入场景的前向轴。

  - **right-handed**

    **命令行长选项:** ***--right-handed***\
    **命令行短选项:** ***-R***

    **默认值:** true

    输入场景使用右手坐标系。（对根运动旋转比较敏感）

## 许可

本工具集以 GPL v3 许可发布。（见 LICENSE）\
本工具集使用了以下库：

- HavokLib, Copyright (c) 2017-2025 Lukas Cone

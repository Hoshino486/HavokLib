# HavokLib

[![build](https://github.com/PredatorCZ/HavokLib/actions/workflows/cmake.yml/badge.svg)](https://github.com/PredatorCZ/HavokLib/actions/workflows/cmake.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Coverage Status](https://coveralls.io/repos/github/PredatorCZ/HavokLib/badge.svg?branch=master)](https://coveralls.io/github/PredatorCZ/HavokLib?branch=master)

HavokLib 是一个独立的 C++ Havok 打包文件（packfile）序列化与 XML 存取库。\
此外，它可以把打包文件格式输出为任意可用版本下的 XML/Bin，并且能够在不同版本之间升级或降级。\
本库也可以保存经典数据，直接导出为 XML。\
本库为 Python 3 模块提供了封装层。\
本库可在 Clang 10、Windows Clang 13.0.1 与 G++10 下编译。

## 界面语言

所有工具默认输出简体中文，并且可以在运行时切回英文，无需重新编译：

| 方式 | 写法 | 优先级 |
| --- | --- | --- |
| 命令行开关 | `havok_toolset hk_to_gltf --language en ...`<br>`-l en` / `--lang en` / `--language=en` | 最高 |
| 环境变量 | `set HAVOK_LANG=en`（也识别 `HK_LANG`、`LANG`） | 次之 |
| 配置文件 | `havok_toolset.config` 的 `<common><language>en</language></common>` | 最低 |

可选值：`zh`、`zh-CN`、`chs`、`中文`、`简体` 表示中文；`en`、`eng`、`english`、`英文` 表示英文。
编译期把 `-DHK_DEFAULT_LANG_EN=1` 传给 CMake，可把默认语言改成英文。

## 工具集

工具集位于 [Toolset 目录](https://github.com/PredatorCZ/HavokLib/tree/master/toolset)

[工具集发行版](https://github.com/PredatorCZ/HavokLib/releases)

## 支持的格式 / 工具集版本

Havok 5.0.0 - 2017

同时支持两种字节序、x86 与 x64 平台，以及为所有可用编译器编译的文件。\
对于 2015 及以后的版本，没有可用于单元测试的样本，只测试过 MSVC 格式！\
本库不支持读取 tagfile，将来也不会支持。\
二进制导出目前仍属实验性功能。

## 支持的类

* hkaSkeleton: 5.0.0 - 2017
* hkRootLevelContainer: 5.0.0 - 2017
* hkaAnimationContainer: 5.0.0 - 2017
* hkaAnimation（非 final）: 5.0.0 - 2017
* hkaDeltaCompressedAnimation: 5.0.0 - 2010
* hkaWaveletCompressedAnimation: 5.0.0 - 2010（无解压器）
* hkaAnnotationTrack: 5.0.0 - 2017
* hkaInterleavedAnimation: 5.0.0 - 2015
* hkaAnimationBinding: 5.0.0 - 2017
* hkxEnvironment: 5.0.0 - 2014
* hkaSplineCompressedAnimation: 5.0.0 - 2017
* hkaDefaultAnimatedReferenceFrame: 5.0.0 - 2014

## 支持的 XML 类

* hkRootLevelContainer
* hkaAnimationContainer
* hkaSkeleton
* hkaAnnotationTrack
* hkaInterleavedAnimation
* hkaAnimationBinding
* hkxEnvironment

## Python 封装类

* hkRootLevelContainer
* hkaAnimationContainer
* hkaSkeleton
* hkxEnvironment

## 二进制导出的类（v5 - v2014）

* hkRootLevelContainer（只读）
* hkaAnimationContainer（只读）
* hkaSkeleton（只读）
* hkxEnvironment（只读）
* hkNamedVariant
* hkaPartition

![Library Diagram](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/predatorcz/havoklib/master/doc/diag.puml)

## 许可

本库以 GPL v3 许可发布。（见 LICENSE.md）

本库使用了以下库：

* PreCore, Copyright (c) 2016-2022 Lukas Cone

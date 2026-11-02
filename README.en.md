# HavokLib (English)

[![build](https://github.com/PredatorCZ/HavokLib/actions/workflows/cmake.yml/badge.svg)](https://github.com/PredatorCZ/HavokLib/actions/workflows/cmake.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Coverage Status](https://coveralls.io/repos/github/PredatorCZ/HavokLib/badge.svg?branch=master)](https://coveralls.io/github/PredatorCZ/HavokLib?branch=master)

HavokLib is an independent Havok packfile serialize and XML store library for C++.\
In addition, it can output packfile formats into XML/Bin under any available version. This library can possibly upgrade/downgrade between versions.\
Library can also store classic data for XML direct export.\
Library uses wrapper for a python 3 module.\
Library is compilable under Clang 10, Windowns Clang 13.0.1 and G++10.

## UI language

Everything is Simplified Chinese by default and can be switched back to English
at runtime, without a rebuild:

| Method | Example | Priority |
| --- | --- | --- |
| CLI switch | `havok_toolset hk_to_gltf --language en ...`<br>`-l en` / `--lang en` / `--language=en` | highest |
| Environment | `set HAVOK_LANG=en` (`HK_LANG` and `LANG` also accepted) | middle |
| Config file | `<common><language>en</language></common>` in `havok_toolset.config` | lowest |

Accepted values: `zh`, `zh-CN`, `chs`, `中文`, `简体` for Chinese; `en`, `eng`,
`english`, `英文` for English. Pass `-DHK_DEFAULT_LANG_EN=1` to CMake to make
English the compile time default.

## Toolset

Toolset can be found in [Toolset folder](https://github.com/PredatorCZ/HavokLib/tree/master/toolset)

[Toolset releases](https://github.com/PredatorCZ/HavokLib/releases)

## Supported formats/toolset versions

Havok 5.0.0 - 2017

It also supports both endians, both x86 and x64 platforms and files compiled for all available compilers.\
For versions 2015+, there aren't any samples for unit tests. Only MSVC formats tested!\
This library doesn't support loading tagfiles and never will.\
Binary exporting is an experimental feature for now.

## Supported classes

* hkaSkeleton: 5.0.0 - 2017
* hkRootLevelContainer: 5.0.0 - 2017
* hkaAnimationContainer: 5.0.0 - 2017
* hkaAnimation (not final): 5.0.0 - 2017
* hkaDeltaCompressedAnimation: 5.0.0 - 2010
* hkaWaveletCompressedAnimation: 5.0.0 - 2010 (/wo decompressor)
* hkaAnnotationTrack: 5.0.0 - 2017
* hkaAnimationBinding: 5.0.0 - 2017
* hkaInterleavedAnimation: 5.0.0 - 2015
* hkxEnvironment: 5.0.0 - 2014
* hkaSplineCompressedAnimation: 5.0.0 - 2017
* hkaDefaultAnimatedReferenceFrame: 5.0.0 - 2014

## Supported XML classes

* hkRootLevelContainer
* hkaAnimationContainer
* hkaSkeleton
* hkaAnnotationTrack
* hkaInterleavedAnimation
* hkaAnimationBinding
* hkxEnvironment

## Python wrapper classes

* hkRootLevelContainer
* hkaAnimationContainer
* hkaSkeleton
* hkxEnvironment

## Binary export classes (v5 - v2014)

* hkRootLevelContainer (read-only)
* hkaAnimationContainer (read-only)
* hkaSkeleton (read-only)
* hkxEnvironment (read-only)
* hkNamedVariant
* hkaPartition

![Library Diagram](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/predatorcz/havoklib/master/doc/diag.puml)

## License

This library is available under GPL v3 license. (See LICENSE.md)

This library uses following libraries:

* PreCore, Copyright (c) 2016-2022 Lukas Cone

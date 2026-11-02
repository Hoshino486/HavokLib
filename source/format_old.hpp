/*  Havok Format Library
    Copyright(C) 2016-2023 Lukas Cone

    This program is free software : you can redistribute it and / or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "hklib/hk_packfile.hpp"
#include "spike/io/bincore_fwd.hpp"

#define hkMagic1 0x57e0e057
#define hkMagic2 0x10c0c010

struct hkxHeader;

struct hkxHeaderlayout {
  /*
  这里实际上就是指针大小：x86 架构为 4 字节，x64 架构为 8 字节。
  */
  uint8 bytesInPointer, littleEndian,
      /*
      派生类的起始位置取代了基类末尾的填充，也就是把填充放到后面之后。
      这项技术只对非 POD 类生效，这一点非常关键，例如：
      struct Base {}; 我们必须为 A 创建一个基类，否则 A 会被当作 POD 类处理，
      这样编译器就会禁止访问填充区域。
        struct A : Base {int i; char c;};
        sizeof (A) == 8
        struct B : A {short s;};
        sizeof (B) == 8
      因此编译后的结构体布局是这样的：
        struct B
        {
                int i;
                char c;
                char pad;
                short s;
        }
      而不是：
        struct B
        {
                int i;
                char c;
                char pad[3];
                short s;
                char pad[2];
        }
      这项技术被 gcc、clang、Mac 编译器等诸多编译器采用，
      MSVC 以及任何 MS 系编译器都没有采用。
      */
      reusePaddingOptimization,
      /*
      空基类的大小只有 1 字节，但启用优化后这个字节不会被传递给派生类，例如：
      struct A {};
      sizeof(A) == 1
      struct B : A {int i;};
      sizeof(B) == 4
      如果 A 是结构体的成员而不是基类，这条规则就不适用，此时它会按 char 处理？
      好在 sn 与 gcc v2 编译器都没有使用这项技术（我甚至连 SN 编译器的资料都
      找不到，笑）。
      */
      emptyBaseClassOptimization;
};

struct hkxSectionHeaderData {
  char sectionTag[20];
  uint32 absoluteDataStart, localFixupsOffset, globalFixupsOffset,
      virtualFixupsOffset, exportsOffset, importsOffset, bufferSize;

  void SwapEndian();
};

struct hkxSectionHeader : hkxSectionHeaderData {

  struct hkxLocalFixup {
    int32 pointer, destination;
    void SwapEndian();
  };

  struct hkxGlobalFixup {
    int32 pointer, sectionid, destination;
    void SwapEndian();
  };

  struct hkxVirtualFixup {
    int32 dataoffset, sectionid, classnameoffset;
    void SwapEndian();
  };

  uint32 sectionID;
  std::string buffer;
  std::vector<hkxLocalFixup> localFixups;
  std::vector<hkxGlobalFixup> globalFixups;
  std::vector<hkxVirtualFixup> virtualFixups;
  IhkPackFile::VirtualClasses virtualClasses;
  hkxHeader *header;
  void Load(BinReaderRef_e rd);
  void LoadBuffer(BinReaderRef_e rd);
  void LinkBuffer();
  void LinkBuffer86();
  void Finalize();
};

struct hkxHeaderData {
  uint32 magic1, magic2, userTag, version;
  hkxHeaderlayout layout;
  int32 numSections, contentsSectionIndex, contentsSectionOffset,
      contentsClassNameSectionIndex, contentsClassNameSectionOffset;
  char contentsVersion[16];
  uint32 flags;
  int16 maxpredicate, predicateArraySizePlusPadding;

  hkxHeaderData()
      : magic1(hkMagic1), magic2(hkMagic2), userTag(), numSections(2),
        contentsSectionIndex(1), contentsSectionOffset(0),
        contentsClassNameSectionIndex(0), contentsClassNameSectionOffset(75),
        flags(), maxpredicate(), predicateArraySizePlusPadding() {}

  void SwapEndian();
};

struct hkxHeader : hkxHeaderData, IhkPackFile {
  hkToolset toolset;

  std::vector<hkxSectionHeader> sections;

  hkxHeader() : toolset(HKUNKVER) {}

  void Load(BinReaderRef_e rd);
  void Save(BinWritterRef wr, const VirtualClasses &classes) const;
  hkxSectionHeader *GetDataSection() { return &sections[contentsSectionIndex]; }
  VirtualClasses &GetAllClasses() override {
    return GetDataSection()->virtualClasses;
  }
  hkToolset GetToolset() const override { return toolset; }

private:
  void GenerateToolset();
};

from hk_base import *

hkaAnimatedReferenceFrameType = NamedType('hkaAnimatedReferenceFrameType', 1, 1)

hkaAnimatedReferenceFrame = ClassData('hkaAnimatedReferenceFrame')
hkaAnimatedReferenceFrame.inherits = [
    hkReferenceObject,
]
hkaAnimatedReferenceFrame.patches = [
    ClassPatch('HK2012_1', ClassPatchType.append,  ClassMember('frameType', hkaAnimatedReferenceFrameType)),
]

if __name__ == "__main__":
    CLASSES = [hkaAnimatedReferenceFrame,]
    print("// 本文件由程序自动生成，请勿手动修改。")
    print('#pragma once')
    print('#include "hk_base.inl"')

    dump_classes(CLASSES)

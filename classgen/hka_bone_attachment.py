from hk_base import *

hkaBoneAttachment = ClassData('hkaBoneAttachment')
hkaBoneAttachment.members = [
    ClassMember('boneFromAttachment', esMatrix44),
    ClassMember('attachment', hkVariant),
    ClassMember('name', Pointer(TYPES.char)),
    ClassMember('boneIndex', TYPES.int16),
]
hkaBoneAttachment.patches = [
    ClassPatch('HK600', ClassPatchType.prepend, ClassMember('skeletonName', Pointer(TYPES.char))),
    ClassPatch('HK700', ClassPatchType.append, Inheritable(hkReferenceObject), ClassPatchType.replace, ClassMember('attachment', Pointer(hkReferenceObject))),
]

BASE.permutators = BASE.permutators[0:BASE.permutators.index('HK2015')]

if __name__ == "__main__":
    CLASSES = [hkaBoneAttachment,]
    print("// 本文件由程序自动生成，请勿手动修改。")
    print('#include "hk_base.inl"')

    dump_classes(CLASSES)

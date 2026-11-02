from hk_base import *

hkxMeshClass = NamedType('hkxMeshClass')
hkaSkeleton = NamedType('hkaSkeleton')

hkaMeshBinding = ClassData('hkaMeshBinding')
hkaMeshBinding.members = [
    ClassMember('mesh', Pointer(hkxMeshClass)),
    ClassMember('skeleton', Pointer(hkaSkeleton)),
    LegacyArray('mappings', hkArray.sub), # int16
    LegacyArray('boneFromSkinMeshTransforms', esMatrix44),
]
hkaMeshBinding.patches = [
    ClassPatch('HK600', ClassPatchType.insert_after, 'mesh', ClassMember('skeletonName', Pointer(TYPES.char))),
    ClassPatch('HK700', ClassPatchType.append, Inheritable(hkReferenceObject), ClassPatchType.replace, hkArray('mappings', hkArray.sub), hkArray('boneFromSkinMeshTransforms', esMatrix44)),
    ClassPatch('HK2011_1', ClassPatchType.insert_after, 'skeletonName', ClassMember('name', Pointer(TYPES.char))),
]

if __name__ == "__main__":
    CLASSES = [hkArray.sub, hkaMeshBinding,]
    print("// 本文件由程序自动生成，请勿手动修改。")
    print('#include "hk_base.inl"')

    dump_classes(CLASSES)

from hka_animation import *

hkaInterleavedAnimation = ClassData('hkaInterleavedAnimation')
hkaInterleavedAnimation.inherits = [
    hkaAnimation,
]
hkaInterleavedAnimation.members = [
    LegacyArray('transforms', hkQTransform),
]
hkaInterleavedAnimation.patches = [
    ClassPatch('HK550', ClassPatchType.append,
               LegacyArray('floats', TYPES.float)),
    ClassPatch('HK700', ClassPatchType.replace, hkArray(
        'transforms', hkQTransform), hkArray('floats', TYPES.float))
]

if __name__ == "__main__":
    CLASSES = [hkaInterleavedAnimation,]
    print("// 本文件由程序自动生成，请勿手动修改。")
    print('#include "hka_animation.inl"')

    dump_classes(CLASSES)

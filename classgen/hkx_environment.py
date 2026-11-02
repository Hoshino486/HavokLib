from hk_base import *

hkxEnvironmentVariable = ClassData('hkxEnvironmentVariable')
hkxEnvironmentVariable.members = [
    ClassMember('name', Pointer(TYPES.char)),
    ClassMember('value', Pointer(TYPES.char))
]

hkxEnvironment = ClassData('hkxEnvironment')
hkxEnvironment.members = [
    hkArray('variables', hkxEnvironmentVariable)
]
hkxEnvironment.patches = [
    ClassPatch('HK700', ClassPatchType.append, Inheritable(hkReferenceObject)),
]

if __name__ == "__main__":
    CLASSES = [hkxEnvironmentVariable, hkxEnvironment,]
    print("// 本文件由程序自动生成，请勿手动修改。")
    print('#include "hk_base.inl"')

    dump_classes(CLASSES)

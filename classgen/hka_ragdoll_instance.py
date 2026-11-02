from hk_base import *

hkpRigidBodyClass = NamedType('hkpRigidBodyClass')
hkpConstraintInstance = NamedType('hkpConstraintInstance')
hkaSkeleton = NamedType('hkaSkeleton')

hkaRagdollInstance = ClassData('hkaRagdollInstance')
hkaRagdollInstance.inherits = [
    hkReferenceObject,
]
hkaRagdollInstance.members = [
    hkArray('rigidBodies', Pointer(hkpRigidBodyClass)),
    hkArray('constraints', Pointer(hkpConstraintInstance)),
    hkArray('boneToRigidBodyMap', TYPES.int32),
    ClassMember('skeleton', Pointer(hkaSkeleton)),
]

BASE.permutators = BASE.permutators[0:BASE.permutators.index('HK2015')]

if __name__ == "__main__":
    CLASSES = [hkaRagdollInstance,]
    print("// 本文件由程序自动生成，请勿手动修改。")
    print('#include "hk_base.inl"')

    dump_classes(CLASSES)

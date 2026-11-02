from hk_base import *

SimpleMapping = NamedType('SimpleMapping', 64, 16)
ChainMapping = NamedType('ChainMapping', 112, 16)
MappingType = NamedType('MappingType', 4, 4)
PartitionMappingRange = NamedType('PartitionMappingRange', 8, 4)

hkaSkeletonMapper = ClassData('hkaSkeletonMapper')
hkaSkeletonMapper.inherits = [
    hkReferenceObject,
]
hkaSkeletonMapper.members = [
    ClassMember('skeletonA', Pointer(TYPES.char)),
    ClassMember('skeletonB', Pointer(TYPES.char)),
    hkArray('simpleMappings', SimpleMapping),
    hkArray('chainMappings', ChainMapping),
    hkArray('unmappedBones', TYPES.int16),
    ClassMember('keepUnmappedLocal', TYPES.bool),
]
hkaSkeletonMapper.patches = [
    ClassPatch('HK700', ClassPatchType.insert_after, 'unmappedBones', ClassMember('extractedMotionMapping', hkQTransform),
               ClassPatchType.insert_after, 'keepUnmappedLocal', ClassMember('mappingType', MappingType)),
    ClassPatch('HK2012_1', ClassPatchType.insert_after, 'skeletonB', hkArray('partitionMap', TYPES.int16),
               hkArray('simpleMappingPartitionRanges', PartitionMappingRange), hkArray('chainMappingPartitionRanges', PartitionMappingRange))
]

BASE.permutators = BASE.permutators[0:BASE.permutators.index('HK2015')]

if __name__ == "__main__":
    CLASSES = [hkaSkeletonMapper,]
    print("// 本文件由程序自动生成，请勿手动修改。")
    print('#include "hk_base.inl"')

    dump_classes(CLASSES)

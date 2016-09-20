#pragma once

enum IcbDescriptorType
{
	Short = 0,
	Long = 1,
	Extended = 2,
	Inline = 3
};

enum IcbFileType
{
	Other = 0,
	Directory = 4,
	File = 5
};

enum ShortAllocDescType
{
	RecordedAndAllocated = 0,
	NotRecordedButAllocated = 1,
	NotRecordedAndNotAllocated = 2,
	NextExtent = 3
};

enum VolumeDescriptorType
{
	VolumeDescriptorTypeSpoaringTable = 0,
	VolumeDescriptorTypePrimaryVolume = 1,
	VolumeDescriptorTypeAnchorVolumePtr = 2,
	VolumeDescriptorTypeVolumePtr = 3,
	VolumeDescriptorTypeImplUseVol = 4,
	VolumeDescriptorTypePartition = 5,
	VolumeDescriptorTypeLogicalVolume = 6,
	VolumeDescriptorTypeUnallocSpace = 7,
	VolumeDescriptorTypeTerminating = 8,
	VolumeDescriptorTypeLogicalVolumeIntegrity = 9,
	VolumeDescriptorTypeFileSet = 256,
	VolumeDescriptorTypeFileId = 257,
	VolumeDescriptorTypeAllocationExtent = 258,
	VolumeDescriptorTypeIndirect = 259,
	VolumeDescriptorTypeTerminal = 260,
	VolumeDescriptorTypeFile = 261,
	VolumeDescriptorTypeExtendedAttributesHeader = 262,
	VolumeDescriptorTypeUnallocatedSpace = 263,
	VolumeDescriptorTypeSpaceBitmap = 264,
	VolumeDescriptorTypePartitionIntegrity = 265,
	VolumeDescriptorTypeExtendedFile = 266,
};
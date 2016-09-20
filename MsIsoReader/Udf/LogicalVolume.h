#pragma once
#include <Windows.h>
#include <atlstr.h>
#include <vector>
#include "UdfString.h"
#include "LongAllocationDescriptor.h"
#include "UdfFileSet.h"

class LogicalVolume
{
public:
	UdfString128 Id;

	LongAllocationDescriptor FileSetLocation;

	std::vector<PartitionMap> PartitionMaps;

	UdfFileSet FileSet;

	int BlockSize;

	CString Name() const
	{
		return Id.GetString();
	}
};
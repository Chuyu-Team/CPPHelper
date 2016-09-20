#pragma once
#include "LongAllocationDescriptor.h"
#include "UdfTime.h"

class UdfFileSet
{
public:
	UdfTime RecordingTime;

	LongAllocationDescriptor RootDirICB;
};
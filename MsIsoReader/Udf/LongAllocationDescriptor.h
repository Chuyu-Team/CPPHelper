#pragma once
#include "Enumerations.h"
#include "LogicalBlockAddress.h"

class LongAllocationDescriptor
{
private:
	int _length;

public:
	int get_Length()
	{
		return _length & 0x3FFFFFFF;
	}

	ShortAllocDescType get_DataType()
	{
		return (ShortAllocDescType)(_length >> 30);
	}

	bool get_IsRecAndAlloc()
	{
		return (get_DataType() == ShortAllocDescType::RecordedAndAllocated);
	}

	LogicalBlockAddress Location;

	void Parse(int start, byte* buffer)
	{
		buffer += start;

		//this.Length = UdfHelper.Get32(start, buffer);
		//this.Location.Parse(start + 4, buffer);

		_length = *(int*)buffer;

		Location.Parse(4,buffer);
	}
};
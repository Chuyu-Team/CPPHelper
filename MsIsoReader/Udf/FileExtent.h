#pragma once
#include "Enumerations.h"

class FileExtent
{
private:
	int _length;

public:
	int get_Length()
	{
		return _length & 0x3FFFFFFF;
	}

	void put_Length(int length)
	{
		_length = length;
	}

	int Position;

	int PartitionReference;

	ShortAllocDescType get_DataType()
	{
		return (ShortAllocDescType)(_length >> 30);
	}

	bool IsRecAndAlloc()
	{
		return get_DataType() == ShortAllocDescType::RecordedAndAllocated;
	}
};
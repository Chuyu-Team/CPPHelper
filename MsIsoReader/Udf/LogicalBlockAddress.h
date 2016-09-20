#pragma once
#include <Windows.h>

class LogicalBlockAddress
{
public:
	int Position;

	int PartitionReference;


	void Parse(int start, byte* buffer)
	{
		buffer += start;

		//this.Position = UdfHelper.Get32(start, buffer);
		//this.PartitionReference = UdfHelper.Get16(start + 4, buffer);

		Position = *(int*)buffer;
		PartitionReference = *(UINT16*)(buffer + 4);
	}
};
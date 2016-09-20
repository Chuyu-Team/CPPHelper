#pragma once
#include <Windows.h>

class ShortAllocationDescriptor
{
public:
	int Length;

	int Position;

	void Parse(int start, byte* buffer)
	{
		buffer += start;
		//this.Length = UdfHelper.Get32(start, buffer);
		//this.Position = UdfHelper.Get32(start + 4, buffer);
		Length=*(int*)buffer;
		Position = *(int*)(buffer+4);
	}
};
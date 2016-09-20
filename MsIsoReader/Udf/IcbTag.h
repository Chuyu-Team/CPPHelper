#pragma once
#include <Windows.h>
#include "Enumerations.h"

class IcbTag
{
public:
	IcbFileType FileType;

	IcbDescriptorType DescriptorType;

	
	bool IsDirectory()
	{
		return (FileType == IcbFileType::Directory);
	}

	void Parse(int start, byte* buffer)
	{
		buffer += start;

		FileType = *(IcbFileType*)(buffer + 11);
		

		auto flags = /*UdfHelper.Get16(18, buffer)*/*(UINT16*)(buffer+18);

		DescriptorType = (IcbDescriptorType)(flags & 3);
	}
};
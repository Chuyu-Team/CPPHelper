#pragma once
#include <Windows.h>
#include "LongAllocationDescriptor.h"
#include "UdfString.h"
#include "VolumeTag.h"

const byte FILEID_CHARACS_Parent = (1 << 3);

class UdfFileInformation
{
private:

	byte _fileCharacteristics;

public:
	UdfString Identifier;

	LongAllocationDescriptor Icb;

	bool IsItLinkParent()
	{
		return (_fileCharacteristics & FILEID_CHARACS_Parent) != 0;
	}

	bool Parse(int start, byte* buffer, int size, int& processed)
	{
		processed = 0;
		if (size < 38)
			return false;

		buffer += start;

		VolumeTag tag;
		tag.Parse(0, buffer, size);
		if (tag.Identifier != VolumeDescriptorType::VolumeDescriptorTypeFileId)
			return false;

		_fileCharacteristics = buffer[18];

		int idLen = buffer[19];

		Icb.Parse(20, buffer);
		//int impLen = UdfHelper.Get16(start + 36, buffer);
		int impLen = *(UINT16*)(buffer + 36);
		if (size < 38 + idLen + impLen)
			return false;

		processed = 38;
		processed += impLen;
		Identifier.Parse(processed, buffer, idLen);
		processed += idLen;
		for (; (processed & 3) != 0; processed++)
		{
			if (buffer[processed] != 0)
				return false;
		}

		return (processed <= size);
	}
};
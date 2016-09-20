#pragma once
#include <Windows.h>

class VolumeTag
{
	/// <summary>
	/// Gets or sets the tag identifier.
	/// </summary>
public:
	int Identifier;

	/// <summary>
	/// Parses the tag information from the buffer.
	/// </summary>
	/// <param name="start">The start index of the tag information.</param>
	/// <param name="buffer">The buffer containing the data.</param>
	/// <param name="size">The number of bytes to read.</param>
	/// <returns>Returns true if the data was parsed sucessfully.</returns>
	bool Parse(int start, byte* buffer, int size)
	{
		buffer += start;

		if (size < 16  || buffer[ 5] != 0)
		{
			return false;
		}

		int sum = 0;
		for (int i = 0; i < 16; i++)
		{
			if (i != 4)
			{
				sum = sum + buffer[ i];
			}
		}

		int m = (sum % 256);
		if (m != buffer[ 4])
		{
			return false;
		}
		Identifier = *(UINT16*)buffer;
		//this.Identifier = UdfHelper.Get16(start, buffer);
		return true;
	}
};
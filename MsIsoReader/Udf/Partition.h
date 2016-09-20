#pragma once
#include <Windows.h>
#include <map>
#include "UdfRecord.h"

typedef std::map<int, UdfRecord*>  Map32;

class Partition
{
public:
	int Number;

	int Position;

	int Length;

	int VolumeIndex;

	Map32 Map;

	Partition()
		:VolumeIndex( -1)
	{
		
	}

	Partition(byte* pBuffer)
		:VolumeIndex(-1)
	{
		Number = *(UINT16*)(22 + pBuffer);
		Position = *(UINT32*)(188 + pBuffer);
		Length = *(UINT32*)(192 + pBuffer);
	}
};

struct PartitionMap
{
	byte Type;
	int PartitionNumber;
	int PartitionIndex;
};

//class Map32 : Dictionary<int, UdfRecord>
//{
//	public bool Set(int key, UdfRecord value) {
//		if (this.ContainsKey(key)) {
//			return false;
//		}
//
//		this.Add(key, value);
//		return true;
//	}
//}
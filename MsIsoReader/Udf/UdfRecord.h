#pragma once
#include <vector>
#include "FileExtent.h"
#include <atlstr.h>
#include "..\ImageRecord.h"
#include "UdfString.h"
#include "IcbTag.h"

class UdfRecord :public ImageRecord
{
public:
	int VolumeIndex;
	int PartitionIndex;
	int Key;
	UdfString Id;
	IcbTag IcbTag;
	long NumLogBlockRecorded;
	UdfTime ATime;
	UdfTime MTime;
	bool IsInline;
	CStringA InlineData;

	std::vector<FileExtent> Extents;
	UdfRecord()
		:VolumeIndex(-1)
		, PartitionIndex(-1)
		, Key(-1)
		, ImageRecord()
	{
	}

	virtual ~UdfRecord()
	{
	}

	bool IsUdf()
	{
		return true;
	}

	bool IsDirectory()
	{
		return IcbTag.IsDirectory();
	}

	bool IsSystemItem()
	{
		if (Id.Data.IsEmpty())
			return true;
		if (Id.Data.GetLength() != 1)
			return _isSystem;
		
		byte b = Id.Data[0];
		_isSystem = (b == 0 || b == 1);
		return _isSystem;
	
	}

	CString get_Name()
	{
		return (_name.IsEmpty() ? _name = Id.GetString() : _name);
	}

	UdfTime get_DateTime()
	{
		return ATime;
	}

	void Parse(byte* buffer)
	{
		//_size = UdfHelper.Get64(56, buffer);
		_size = *(UINT64*)(buffer + 56);

		//NumLogBlockRecorded = UdfHelper.Get64(64, buffer);
		NumLogBlockRecorded = *(UINT64*)(buffer + 64);
		ATime.Parse(72, buffer);
		MTime.Parse(84, buffer);
	}

	bool CheckChunkSizes() 
	{
		return GetChunksSumSize() == get_Size();
	}

	bool IsRecAndAlloc()
	{
		/*for (int i = 0; i < Extents.size(); i++)
			if (!Extents[i].IsRecAndAlloc)
				return false;*/

		for (auto& Item : Extents)
		{
			if(!Item.IsRecAndAlloc())
				return false;
		}

		return true;
	}

	long GetChunksSumSize()
	{
		if (IsInline)
			return InlineData.GetLength();
		long size = 0;

		/*for (int i = 0; i < Extents.Count; i++)
			size += Extents[i].Length;*/
		for (auto& Item : Extents)
		{
			size += Item.get_Length();
		}
		return size;
	}

	void Clear()
	{
		Extents.clear();
		pParent = NULL;
		Id = UdfString();
		//IcbTag = IcbTag();
		memset(&IcbTag,NULL,sizeof(IcbTag));
		ATime = UdfTime();
		MTime = UdfTime();
		NumLogBlockRecorded = 0;
		IsInline = false;
		InlineData.Empty();
		ImageRecord::Clear();
	}
};
#pragma once
#include <vector>
#include "Udf\Partition.h"
#include "Udf\LogicalVolume.h"
#include "ImageRecord.h"
#include "Udf\UdfFileExtent.h"
#include "Udf\UdfRecord.h"
#include "Udf\VolumeTag.h"
#include "Udf\UdfFileInformation.h"
#include "..\ComHelper.h"
#include <imapi2fs.h>

const int SectorSizeLog = 11;
const int SectorSize = 1 << SectorSizeLog;
const int VirtualSectorSize = 512;
const int PrimaryVolumeSector = 16;
const int BootRecordVolumeSector = 17;

const int MaxPartitions = 64;
const int MaxLogicalVolumes = 64;
const int MaxRecurseLevels = 1024;
const int MaxItems = 134217728;
const int MaxFiles = 268435456;
const int MaxExtents = 1073741824;
const int MaxFileNameLength = 858934592;
const int MaxInlineExtentsSize = 858934592;

const int BufferSize = SectorSize * 4096;

//从一个一个现有流偏移产生一个新的流
class CStreamOffect :public IUnknownT<CStreamOffect, IStream>
{
public:
	unsigned long long Size;
	long long Start;
	IStream* pStream;

	CStreamOffect(IStream* _pStream, long long _Start, unsigned long long _Size)
		:Size(_Size)
		, Start(_Start)
		, pStream(_pStream)
	{
		pStream->AddRef();
	}

	virtual ~CStreamOffect()
	{
		pStream->Release();
	}


	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
		/* [annotation] */
		_Out_writes_bytes_to_(cb, *pcbRead)  void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbRead)
	{
		return pStream->Read(pv, cb, pcbRead);
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write(
		/* [annotation] */
		_In_reads_bytes_(cb)  const void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbWritten)
	{
		return pStream->Write(pv, cb, pcbWritten);
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek(
		/* [in] */ LARGE_INTEGER dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [annotation] */
		_Out_opt_  ULARGE_INTEGER *plibNewPosition)
	{
		switch (dwOrigin)
		{
		case FILE_END:
			dlibMove.QuadPart = Size + dlibMove.QuadPart;
			dwOrigin = FILE_BEGIN;
			break;
		case FILE_BEGIN:
			dlibMove.QuadPart += Start;
			break;
		default:
			break;
		}

		return pStream->Seek(dlibMove, dwOrigin, plibNewPosition);
	}

	virtual HRESULT STDMETHODCALLTYPE SetSize(
		/* [in] */ ULARGE_INTEGER libNewSize)
	{
		return E_NOINTERFACE;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE CopyTo(
		/* [annotation][unique][in] */
		_In_  IStream *pstm,
		/* [in] */ ULARGE_INTEGER cb,
		/* [annotation] */
		_Out_opt_  ULARGE_INTEGER *pcbRead,
		/* [annotation] */
		_Out_opt_  ULARGE_INTEGER *pcbWritten)
	{
		return E_NOINTERFACE;
	}

	virtual HRESULT STDMETHODCALLTYPE Commit(
		/* [in] */ DWORD grfCommitFlags)
	{
		return E_NOINTERFACE;
	}

	virtual HRESULT STDMETHODCALLTYPE Revert(void)
	{
		return E_NOINTERFACE;
	}

	virtual HRESULT STDMETHODCALLTYPE LockRegion(
		/* [in] */ ULARGE_INTEGER libOffset,
		/* [in] */ ULARGE_INTEGER cb,
		/* [in] */ DWORD dwLockType)
	{
		return E_NOINTERFACE;
	}

	virtual HRESULT STDMETHODCALLTYPE UnlockRegion(
		/* [in] */ ULARGE_INTEGER libOffset,
		/* [in] */ ULARGE_INTEGER cb,
		/* [in] */ DWORD dwLockType)
	{
		return E_NOINTERFACE;
	}

	virtual HRESULT STDMETHODCALLTYPE Stat(
		/* [out] */ __RPC__out STATSTG *pstatstg,
		/* [in] */ DWORD grfStatFlag)
	{
		return E_NOINTERFACE;
	}

	virtual HRESULT STDMETHODCALLTYPE Clone(
		/* [out] */ __RPC__deref_out_opt IStream **ppstm)
	{
		return E_NOINTERFACE;
	}
};


/*
从微软移植的ISO读取库
*/


class ImageReader
{
private:
	std::vector<Partition> Partitions;
	std::vector<LogicalVolume> LogicalVolumes;

	//int currentBlockSize = SectorSize;
	IStream* stream;
	ULARGE_INTEGER imageSize;
	long fileNameLengthTotal;
	int numExtents;
	long inlineExtentsSize;
	int itemCount;

	//long bytesExtracted;


	/// <summary>
	/// Gets or sets the root directory of the image.
	/// </summary>
	UdfRecord RootDirectory;

public:
	ImageReader(IStream* fileStream)
		:stream(fileStream)
		, fileNameLengthTotal(0)
		, numExtents(0)
		, inlineExtentsSize(0)
		, itemCount(0)
	{
		stream->AddRef();
	}

	~ImageReader()
	{
		stream->Release();
	}

	/// <summary>
	/// Opens the file and attempts to read the contents of the ISO image.
	/// </summary>
	/// <returns>Returns true if the image was opened successfully.</returns>
	bool Initialize()
	{

		// Initialize the stream
		stream->Seek(LARGE_INTEGER{}, FILE_END, &imageSize);

		// Must have at least one sector in the image.
		if (imageSize.QuadPart < SectorSize)
		{
			return false;
		}

		// Parse the anchor pointer to find the location of the volume descriptors.
		UdfFileExtent extentVds;

		if (!ReadAnchorVolumePointer(extentVds))
		{
			return false;
		}

		// Parse the volume and paritiotion information from the image.
		if (!ReadVolumeDescriptors(extentVds))
		{
			return false;
		}

		// Finally, read the file structure.
		return ReadFileStructure();
	}

	/// <summary>
	/// Extracts files from the image to the given directory.
	/// </summary>
	/// <param name="path">The directory to extract the files to.</param>
	/// <param name="root">The root directory of the image.</param>
	/*void ExtractFiles(LPCWSTR path, ImageRecord root)
	{
	if (String.IsNullOrEmpty(path))
	{
	throw new ArgumentNullException("path");
	}

	if (root == null)
	{
	throw new ArgumentNullException("root");
	}

	if (this.ImageFile == null)
	{
	throw new InvalidOperationException("No image file specified.");
	}

	using (var fileStream = this.ImageFile.OpenRead())
	{
	this.InitializeStream(fileStream);

	this.Extract(path, root);
	}
	}*/

	//打开现有文件流
	HRESULT IsoOpenFile(LPCWSTR hFilePath, IStream** ppStream)
	{
		return IsoOpenFile(&RootDirectory, hFilePath, ppStream);
	}

	HRESULT IsoOpenFile(ImageRecord* pRootDirectory, LPCWSTR hFilePath, IStream** ppStream)
	{
		for (auto& SubItem : pRootDirectory->_SubItems)
		{
			auto name = SubItem->get_Name();

			if (SubItem->IsDirectory())
			{
				name += L'\\';

				if (wcsnicmp(name, hFilePath, name.GetLength()) == 0)
				{
					return IsoOpenFile(SubItem, hFilePath + name.GetLength(), ppStream);
				}
			}
			else
			{
				if (wcsicmp(name, hFilePath) == 0)
				{
					//找到的文件

					//只支持释放UDF文件
					if (!SubItem->IsUdf())
					{
						return E_NOINTERFACE;
					}

					auto pItem = (UdfRecord*)SubItem;

					if (pItem->IsInline)
					{
						//文件存在内存中

						*ppStream = SHCreateMemStream((byte*)pItem->InlineData.GetBuffer(), pItem->InlineData.GetLength());

					}
					else
					{
						auto& part = Partitions[pItem->PartitionIndex];
						auto currentBlockSize = LogicalVolumes[pItem->VolumeIndex].BlockSize;
						long logBlockNumber = pItem->Key;
						if (pItem->Extents.size())
						{
							logBlockNumber = pItem->Extents[0].Position;
						}

						auto start = ((long long)part.Position << SectorSizeLog) + ((long long)logBlockNumber * currentBlockSize);

						*ppStream = new CStreamOffect(stream, start, pItem->get_Size());
					}


					return S_OK;
				}
			}
		}

		return ERROR_FILE_NOT_FOUND;
	}


	/// <summary>
	/// Extracts an individual record from the image to the given directory.
	/// </summary>
	/// <param name="path">The directory to extract the record to.</param>
	/// <param name="record">The record to extract.</param>
	void Extract(CString path, ImageRecord* record)
	{
		if (path.IsEmpty())
			return;


		// Only extract UDF records.
		if (!record->IsUdf())
		{
			return;
		}

		if (path[path.GetLength() - 1] != L'\\')
			path += L'\\';


		CString target = path + record->get_Name();

		if (record->IsDirectory())
		{
			CreateDirectory(target, NULL);

			// No sub items for this directory, continue.

			for (auto& SubItem : record->_SubItems)
			{
				Extract(target, SubItem);
			}
		}
		else
		{
			auto item = (UdfRecord*)record;
			if (item->IsRecAndAlloc() && item->CheckChunkSizes() && CheckItemExtents(item->VolumeIndex, item))
			{
				if (item->IsInline)
				{
					/*if (item.InlineData != null)
					{
					if (item.InlineData.Length == 0)
					{
					return;
					}

					File.WriteAllBytes(target, item.InlineData);
					}*/


					auto hFile = CreateFile(target, 0, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

					if (hFile != INVALID_HANDLE_VALUE)
					{
						DWORD cbData;

						WriteFile(hFile, item->InlineData.GetBuffer(), item->InlineData.GetLength(), &cbData, NULL);

						CloseHandle(hFile);
					}

				}
				else
				{
					auto& part = Partitions[item->PartitionIndex];
					auto currentBlockSize = LogicalVolumes[item->VolumeIndex].BlockSize;
					long logBlockNumber = item->Key;
					if (item->Extents.size())
					{
						logBlockNumber = item->Extents[0].Position;
					}

					long start = ((long)part.Position << SectorSizeLog) + (logBlockNumber * currentBlockSize);
					Extract(target, start, item->get_Size());
				}
			}
		}
	}

	/// <summary>
	/// Extracts a file from the image.
	/// </summary>
	/// <param name="path">The target path of the file.</param>
	/// <param name="start">The start location of the file in the image.</param>
	/// <param name="length">The length of the file.</param>
	void Extract(LPCWSTR path, long start, long length)
	{
		// Overwrite the file if it already exists.
		auto hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		if (hFile == INVALID_HANDLE_VALUE)
			return;

		// Seek stream to start location
		stream->Seek(LARGE_INTEGER{ (DWORD)start }, FILE_BEGIN, NULL);

		// Write the bits to the file.

		auto buffer = new byte[BufferSize];
		ULONG bytesRead;
		while (length > 0)
		{
			DWORD sizeToRead = (length < BufferSize) ? (int)length : BufferSize;

			stream->Read(buffer, sizeToRead, &bytesRead);

			WriteFile(hFile, buffer, sizeToRead, &sizeToRead, NULL);
			//fs.Write(buffer, 0, bytesRead);
			length -= bytesRead;
		}

		CloseHandle(hFile);
	}



	/// <summary>
	/// Reads the Anchor Volume pointer from the image.
	/// </summary>
	/// <returns>Returns true if the pointer was found.</returns>
	bool ReadAnchorVolumePointer(UdfFileExtent& result)
	{
		byte buffer[SectorSize];

		LARGE_INTEGER dlibMove = {};
		dlibMove.QuadPart = -1ll * sizeof(buffer);

		auto hr = stream->Seek(dlibMove, FILE_END, NULL);
		if (stream->Read(buffer, sizeof(buffer), NULL))
		{
			return false;
		}

		VolumeTag tag;
		if (tag.Parse(0, buffer, sizeof(buffer))
			&& tag.Identifier == (short)VolumeDescriptorType::VolumeDescriptorTypeAnchorVolumePtr)
		{
			result.Parse(16, buffer);
			return true;
		}

		return false;
	}

	/// <summary>
	/// Reads the volume descriptors for
	/// </summary>
	/// <param name="extentVds">The anchor volume extent information.</param>
	/// <returns>Returns true if the volume descriptors were read from the image successfully.</returns>
	bool ReadVolumeDescriptors(UdfFileExtent& extentVds)
	{
		byte buffer[SectorSize];

		long location = extentVds.Position;
		LARGE_INTEGER dlibMove;
		VolumeTag tag;

		while (location < extentVds.Length && location < imageSize.QuadPart)
		{
			dlibMove.QuadPart = location << SectorSizeLog;
			stream->Seek(dlibMove, FILE_BEGIN, NULL);
			//if (!this.stream.ReadSafe(buffer, buffer.Length))
			if (stream->Read(buffer, sizeof(buffer), NULL))
			{
				return false;
			}
			if (!tag.Parse(0, buffer, sizeof(buffer)))
				return false;

			switch ((VolumeDescriptorType)tag.Identifier)
			{
			case VolumeDescriptorType::VolumeDescriptorTypeTerminating:
				// Found terminating descriptor.  Image is valid.
				return true;
			case VolumeDescriptorType::VolumeDescriptorTypePartition:
				if (Partitions.size() >= MaxPartitions)
				{
					return false;
				}

				ReadPartitionDescriptor(buffer);
				break;
			case VolumeDescriptorType::VolumeDescriptorTypeLogicalVolume:
				if (LogicalVolumes.size() >= MaxLogicalVolumes || !ReadLogicalDescriptor(buffer))
				{
					return false;
				}

				break;
			}

			location++;
		}

		// Did not find the terminating descriptor.  Not a valid image.
		return false;
	}

	/// <summary>
	/// Reads a partition descriptor from the buffer.
	/// </summary>
	/// <param name="buffer">The buffer to read the partition data from.</param>
	void ReadPartitionDescriptor(byte* buffer)
	{
		Partitions.push_back(Partition(buffer));
	}

	/// <summary>
	/// Reads a logical volume descriptor from the buffer.
	/// </summary>
	/// <param name="buffer">The buffer to read the data from.</param>
	/// <returns>Returns true if the descriptor is valid.</returns>
	bool ReadLogicalDescriptor(byte* buffer)
	{
		LogicalVolume volume;
		volume.Id.Parse(84, buffer);

		volume.BlockSize = *(UINT32*)(212 + buffer);
		if (volume.BlockSize < VirtualSectorSize || volume.BlockSize > MaxExtents)
		{
			return false;
		}

		volume.FileSetLocation.Parse(248, buffer);

		auto numPartitionMaps = *(UINT32*)(268 + buffer);
		if (numPartitionMaps > MaxPartitions)
		{
			return false;
		}

		int position = 440;
		PartitionMap pm;

		for (int index = 0; index < numPartitionMaps; index++)
		{
			if (position + 2 > SectorSize)
			{
				return false;
			}


			pm.Type = buffer[position];
			byte length = buffer[position + 1];
			if (position + length > SectorSize)
			{
				return false;
			}

			if (pm.Type == 1)
			{
				if (position + 6 > SectorSize)
				{
					return false;
				}

				pm.PartitionNumber = *(UINT16*)(position + 4 + buffer);
			}
			else
			{
				return false;
			}

			position += length;
			pm.PartitionIndex = volume.PartitionMaps.size();
			volume.PartitionMaps.push_back(pm);
		}

		LogicalVolumes.push_back(volume);
		return true;
	}

	/// <summary>
	/// Validates that the volume has a valid partition.
	/// </summary>
	/// <param name="volume">The volume to validate.</param>
	/// <returns>Returns true if the volume is valid.</returns>
	bool ValidateVolumePartition(LogicalVolume& volume)
	{
		for (int i = 0; i < volume.PartitionMaps.size(); i++)
		{
			PartitionMap& map = volume.PartitionMaps[0];

			bool found = false;
			//foreach(var partition in this.Partitions)
			for (int j = 0;j != Partitions.size();++j)
			{
				auto& partition = Partitions[j];
				if (partition.Number == map.PartitionNumber)
				{
					// partition can only be member of one volume
					if (partition.VolumeIndex >= 0)
					{
						return false;
					}

					// Add cross references between partitions and volumes.
					map.PartitionNumber = j;
					partition.VolumeIndex = i;
					found = true;
				}
			}

			if (!found)
			{
				return false;
			}
		}

		return true;
	}

	/// <summary>
	/// Reads the file and directory structure from the image.
	/// </summary>
	/// <returns>Returns true if the file system is valid.</returns>
	bool ReadFileStructure()
	{
		//foreach(var volume in this.LogicalVolumes)
		CStringA Buffer;
		VolumeTag tag;
		for (int i = 0;i != LogicalVolumes.size();++i)
		{
			auto&volume = LogicalVolumes[i];
			// Ensure the volume and parittion are valid.
			if (!ValidateVolumePartition(volume))
			{
				return false;
			}

			int& volIndex = i;
			LongAllocationDescriptor& nextExtent = volume.FileSetLocation;
			auto Length = nextExtent.get_Length();
			if (Length < VirtualSectorSize)
			{
				return false;
			}

			//byte[] buffer = new byte[nextExtent.Length];


			ReadData(volIndex, nextExtent, Buffer);
			auto buffer = (byte*)Buffer.GetBuffer();

			tag.Parse(0, (byte*)buffer, Length);
			if (tag.Identifier != (int)VolumeDescriptorType::VolumeDescriptorTypeFileSet)
			{
				return false;
			}

			//UdfFileSet fs;
			//fs.RecordingTime.Parse(16, buffer);
			//fs.RootDirICB.Parse(400, buffer);
			volume.FileSet.RecordingTime.Parse(16, buffer);
			volume.FileSet.RootDirICB.Parse(400, buffer);
			if (!ReadRecord((UdfRecord*)&RootDirectory, volIndex, volume.FileSet.RootDirICB, MaxRecurseLevels))
			{
				return false;
			}
		}

		return true;
	}

	/// <summary>
	/// Reads a record from the file system.
	/// </summary>
	/// <param name="item">The item to read the data into.</param>
	/// <param name="volumeIndex">The index of the volume the file resides on.</param>
	/// <param name="lad">The long allocation descriptor of the file.</param>
	/// <param name="numRecurseAllowed">The number of recursions allowed before the method fails.</param>
	/// <returns>Returns true if the item and all sub items were read correctly.</returns>
	bool ReadRecord(UdfRecord* item, int volumeIndex, LongAllocationDescriptor& lad, int numRecurseAllowed)
	{
		if (numRecurseAllowed-- == 0)
		{
			return false;
		}

		auto& vol = LogicalVolumes[volumeIndex];
		auto& partition = Partitions[vol.PartitionMaps[lad.Location.PartitionReference].PartitionIndex];

		int& key = lad.Location.Position;

		auto T = partition.Map.find(key);

		if (T != partition.Map.end())
		{
			// Item already in the map, just look it up instead of reading it from the image.
			item->VolumeIndex = T->second->VolumeIndex;
			item->PartitionIndex = T->second->PartitionIndex;
			item->Extents = T->second->Extents;
			item->_size = T->second->_size;
			item->Key = key;
		}
		else
		{
			item->VolumeIndex = volumeIndex;
			item->PartitionIndex = vol.PartitionMaps[lad.Location.PartitionReference].PartitionIndex;
			item->Key = key;

			partition.Map[key] = item;
			if (!ReadRecordData(item, volumeIndex, lad, numRecurseAllowed))
			{
				return false;
			}
		}

		return true;
	}

	/// <summary>
	/// Reads the data from the image for a record.
	/// </summary>
	/// <param name="item">The item to read the data into.</param>
	/// <param name="volumeIndex">The index of the volume the file resides on.</param>
	/// <param name="lad">The long allocation descriptor of the file.</param>
	/// <param name="numRecurseAllowed">The number of recursions allowed before the method fails.</param>
	/// <returns>Returns true if the record was read successfully.</returns>
	bool ReadRecordData(UdfRecord* item, int volumeIndex, LongAllocationDescriptor& lad, int numRecurseAllowed)
	{
		if (itemCount > MaxItems)
		{
			return false;
		}

		auto& volume = LogicalVolumes[volumeIndex];
		auto size = lad.get_Length();
		if (size != volume.BlockSize)
		{
			return false;
		}

		// Read the record.
		CStringA Buffer;
		//
		if (!ReadData(volumeIndex, lad, Buffer))
		{
			return false;
		}
		auto buffer = (byte*)Buffer.GetBuffer();
		// Validate the tag is a file.
		VolumeTag tag;
		tag.Parse(0, buffer, size);
		if (tag.Identifier != (short)VolumeDescriptorType::VolumeDescriptorTypeFile)
		{
			return false;
		}

		// Validate the IcbTage indicates file or directory.
		item->IcbTag.Parse(16, buffer);
		if (item->IcbTag.FileType != IcbFileType::Directory && item->IcbTag.FileType != IcbFileType::File)
		{
			return false;
		}

		item->Parse(buffer);
		int extendedAttrLen = *(UINT32*)(168 + buffer);
		int allocDescriptorsLen = *(UINT32*)(172 + buffer);
		if ((extendedAttrLen & 3) != 0)
		{
			return false;
		}

		int position = 176;
		if (extendedAttrLen > size - position)
		{
			return false;
		}

		position += extendedAttrLen;
		IcbDescriptorType& desctType = item->IcbTag.DescriptorType;
		if (allocDescriptorsLen > size - position)
		{
			return false;
		}

		if (desctType == IcbDescriptorType::Inline)
		{
			// If the file data is inline, read it in now since we have it.
			item->IsInline = true;
			//item->InlineData = UdfHelper.Readbytes(position, buffer, allocDescriptorsLen);
			item->InlineData.SetString((char*)buffer + position, allocDescriptorsLen);
		}
		else
		{
			// Otherwise read the information about where the file is located for later.
			item->IsInline = false;
			if ((desctType != IcbDescriptorType::Short) && (desctType != IcbDescriptorType::Long))
			{
				return false;
			}
			FileExtent extent;
			for (int index = 0; index < allocDescriptorsLen;)
			{

				if (desctType == IcbDescriptorType::Short)
				{
					if (index + 8 > allocDescriptorsLen)
					{
						return false;
					}

					ShortAllocationDescriptor sad;
					sad.Parse(position + index, buffer);

					extent.Position = sad.Position;
					extent.put_Length(sad.Length);
					extent.PartitionReference = lad.Location.PartitionReference;
					index += 8;
				}
				else
				{
					if (index + 16 > allocDescriptorsLen)
					{
						return false;
					}

					LongAllocationDescriptor ladNew;
					ladNew.Parse(position + index, buffer);
					extent.Position = ladNew.Location.Position;
					extent.PartitionReference = ladNew.Location.PartitionReference;
					extent.put_Length(ladNew.get_Length());
					index += 16;
				}

				item->Extents.push_back(extent);
			}
		}

		if (item->IcbTag.IsDirectory())
		{
			if (!item->CheckChunkSizes() || !CheckItemExtents(volumeIndex, item))
			{
				return false;
			}

			//buffer = new byte[0];
			if (!ReadFromFile(volumeIndex, item, Buffer))
			{
				return false;
			}
			buffer = (byte*)Buffer.GetBuffer();
			item->_size = 0;
			item->Extents.clear();
			size = Buffer.GetLength();

			int processedTotal = 0;
			int processedCur = -1;
			while (processedTotal < size || processedCur == 0)
			{
				UdfFileInformation fileId;
				fileId.Parse(processedTotal, buffer, (size - processedTotal), processedCur);
				if (!fileId.IsItLinkParent())
				{
					// Recursively read the contentst of the drirectory
					UdfRecord* fileItem = new UdfRecord;
					fileItem->Id = fileId.Identifier;
					if (fileItem->Id.Data.GetLength())
					{
						fileNameLengthTotal += fileItem->Id.Data.GetLength();
					}

					if (fileNameLengthTotal > MaxFileNameLength)
					{
						delete fileItem;
						return false;
					}

					fileItem->pParent = item;
					item->_SubItems.push_back(fileItem);
					if (item->_SubItems.size() > MaxFiles)
					{
						return false;
					}

					ReadRecord(fileItem, volumeIndex, fileId.Icb, numRecurseAllowed);
				}

				processedTotal += processedCur;
			}
		}
		else
		{
			if (item->Extents.size() > MaxExtents - numExtents)
			{
				return false;
			}

			numExtents += item->Extents.size();
			if (item->InlineData.GetLength() > MaxInlineExtentsSize - inlineExtentsSize)
			{
				return false;
			}

			inlineExtentsSize += item->InlineData.GetLength();
		}

		itemCount++;
		return true;
	}

	/// <summary>
	/// Reads data from the file.
	/// </summary>
	/// <param name="volumeIndex">The volume index of the file.</param>
	/// <param name="item">The item containing information about the file to read.</param>
	/// <param name="buffer">The buffer to read the data</param>
	/// <returns>Returns true if the data was read successfully.</returns>
	bool ReadFromFile(int volumeIndex, UdfRecord* item, CStringA& buffer)
	{
		buffer.Empty();
		auto Size = item->get_Size();
		if (Size >= MaxExtents)
		{
			return false;
		}

		if (item->IsInline)
		{
			buffer = item->InlineData;
			return true;
		}

		auto pBuffer = (byte*)buffer.GetBuffer(Size);
		buffer.ReleaseBufferSetLength(Size);
		//buffer = new byte[item.Size];
		int position = 0;
		//for (int i = 0; i < item.Extents.Count; i++)
		for (auto& e : item->Extents)
		{
			int length = e.get_Length();
			//byte[] b = UdfHelper.Readbytes(position, buffer, buffer.Length);
			if (!ReadData(volumeIndex, e.PartitionReference, e.Position, length, pBuffer + position))
			{
				return false;
			}

			position += length;
		}

		return true;
	}

	/// <summary>
	/// Reads data from the stream.
	/// </summary>
	/// <param name="volumeIndex">The volume index of the data.</param>
	/// <param name="lad">The long allocation descriptor of the data.</param>
	/// <param name="buffer">The buffer to contain the data.</param>
	/// <returns>Returns true if the data was read successfully.</returns>
	bool ReadData(int volumeIndex, LongAllocationDescriptor& lad, CStringA& buffer)
	{
		buffer.Empty();
		auto cbData = lad.get_Length();

		auto ret = ReadData(volumeIndex, lad.Location.PartitionReference, lad.Location.Position, cbData, (byte*)buffer.GetBuffer(cbData));
		if (ret)
		{
			buffer.ReleaseBufferSetLength(cbData);
		}

		return ret;
	}

	/// <summary>
	/// Reads data from the stream.
	/// </summary>
	/// <param name="volumeIndex">The volume index of the data.</param>
	/// <param name="partitionReference">The partition reference.</param>
	/// <param name="blockPosition">The block position of the data to read.</param>
	/// <param name="length">The length of the data to read.</param>
	/// <param name="buffer">The buffer to contain the data.</param>
	/// <returns>Returns true if the data was read successfully.</returns>
	bool ReadData(int volumeIndex, int partitionReference, int blockPosition, int length, byte* buffer)
	{
		if (!CheckExtent(volumeIndex, partitionReference, blockPosition, length))
		{
			return false;
		}

		auto& volume = LogicalVolumes[volumeIndex];
		auto& partition = Partitions[volume.PartitionMaps[partitionReference].PartitionIndex];
		LARGE_INTEGER dlibMove;

		dlibMove.QuadPart = ((long long)partition.Position << SectorSizeLog) + ((long long)blockPosition * volume.BlockSize);
		stream->Seek(dlibMove, FILE_BEGIN, NULL);
		return !stream->Read(buffer, length, NULL);
	}

	/// <summary>
	/// Validates to ensure all item extents are in a valid format.
	/// </summary>
	/// <param name="volumeIndex">The volume index.</param>
	/// <param name="item">The item to validate.</param>
	/// <returns>Returns true if the extents are valid.</returns>
	bool CheckItemExtents(int volumeIndex, UdfRecord* item)
	{
		//foreach(FileExtent extent in item.Extents)
		for (auto& extent : item->Extents)
		{
			if (!CheckExtent(volumeIndex, extent.PartitionReference, extent.Position, extent.get_Length()))
			{
				return false;
			}
		}

		return true;
	}

	/// <summary>
	/// Validates to ensure the extent is in a valid format.
	/// </summary>
	/// <param name="volumeIndex">The volume index.</param>
	/// <param name="partitionReference">The partition reference.</param>
	/// <param name="blockPosition">The current block position.</param>
	/// <param name="length">The length of the extent.</param>
	/// <returns>Returns true if the extent is valid.</returns>
	bool CheckExtent(int volumeIndex, int partitionReference, int blockPosition, int length)
	{
		auto& volume = LogicalVolumes[volumeIndex];
		auto& partition = Partitions[volume.PartitionMaps[partitionReference].PartitionIndex];
		auto offset = ((long long)partition.Position << SectorSizeLog) + ((long long)blockPosition * volume.BlockSize);

		return (offset + length) <= (((long long)partition.Position + partition.Length) << SectorSizeLog);
	}
};
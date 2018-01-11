#pragma once
#include "BaseFunction.h"
#include <virtdisk.h>
#pragma comment(lib,"VirtDisk.lib")
#include <Sddl.h>

_Check_return_ _Success_(return != INVALID_HANDLE_VALUE)
static HANDLE AttachVirtualDisk(
	_In_z_ LPCWSTR VirtualDiskPath,
	_In_ bool ReadOnly
	)
{
	OPEN_VIRTUAL_DISK_PARAMETERS openParameters = {};
	VIRTUAL_DISK_ACCESS_MASK accessMask;

	if (StrCmpI(PathFindExtension(VirtualDiskPath), L".iso") == 0)
	{
		openParameters.Version = OPEN_VIRTUAL_DISK_VERSION_1;

		accessMask = VIRTUAL_DISK_ACCESS_READ;
	}
	else
	{
		openParameters.Version = OPEN_VIRTUAL_DISK_VERSION_2;

		openParameters.Version2.GetInfoOnly = FALSE;

		accessMask = VIRTUAL_DISK_ACCESS_NONE;
	}
	
	HANDLE vhdHandle = INVALID_HANDLE_VALUE;
	VIRTUAL_STORAGE_TYPE storageType = { VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN  };


	auto opStatus = OpenVirtualDisk(

		&storageType,

		VirtualDiskPath,

		accessMask,

		OPEN_VIRTUAL_DISK_FLAG_NONE,

		&openParameters,

		&vhdHandle);

	if (opStatus)
	{
		SetLastError(opStatus);
		return INVALID_HANDLE_VALUE;
	}
	PSECURITY_DESCRIPTOR sd = NULL;

	if (!::ConvertStringSecurityDescriptorToSecurityDescriptor(

		L"O:BAG:BAD:(A;;GA;;;WD)",

		SDDL_REVISION_1,

		&sd,

		NULL))

	{

		opStatus = ::GetLastError_s();

		goto Cleanup;
	}


	ATTACH_VIRTUAL_DISK_PARAMETERS attachParameters = { ATTACH_VIRTUAL_DISK_VERSION_1 };
	ATTACH_VIRTUAL_DISK_FLAG attachFlags= ReadOnly? (ATTACH_VIRTUAL_DISK_FLAG_NO_DRIVE_LETTER| ATTACH_VIRTUAL_DISK_FLAG_READ_ONLY) : ATTACH_VIRTUAL_DISK_FLAG_NO_DRIVE_LETTER;

	
	opStatus = AttachVirtualDisk(

		vhdHandle,

		sd,

		attachFlags,

		0,

		&attachParameters,

		NULL);



	if (opStatus != ERROR_SUCCESS)

	{
		goto Cleanup;
	}
	else
	{
		return vhdHandle;
	}


Cleanup:
	CloseHandle(vhdHandle);
	LocalFree(sd);
	SetLastError(opStatus);
	return INVALID_HANDLE_VALUE;
}
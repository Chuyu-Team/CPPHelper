#pragma once
#include <Windows.h>
#include <winioctl.h>

_Check_return_ _Success_(return == S_OK)
HRESULT DiskUpdateBootCode(
	_In_z_ LPCWSTR BootPartition
	);

_Check_return_ _Success_(return == S_OK)
HRESULT DiskGetPartitionStyle(
	_In_z_ LPCWSTR          Partition,
	_Out_  PARTITION_STYLE* pPartitionStyle
	);
#pragma once
#include <Windows.h>
#include <winioctl.h>

_Check_return_
LSTATUS DiskUpdateBootCode(
	_In_z_ LPCWSTR BootPartition
	);

_Check_return_
LSTATUS DiskGetPartitionStyle(
	_In_z_ LPCWSTR          Partition,
	_Out_  PARTITION_STYLE* pPartitionStyle
	);
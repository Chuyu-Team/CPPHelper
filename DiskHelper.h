#pragma once
#include <Windows.h>
#include <winioctl.h>

HRESULT DiskUpdateBootCode(LPCWSTR BootPartition);

HRESULT DiskGetPartitionStyle(LPCWSTR Partition, PARTITION_STYLE* pPartitionStyle);
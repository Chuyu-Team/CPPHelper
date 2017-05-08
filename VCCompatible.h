#pragma once

//此头文件用于兼容老版本编译器支持
#include <sal.h>

#if PlatformToolset==90
#undef _MSC_VER
#define _MSC_VER  1500
#elif PlatformToolset ==140
#undef _MSC_VER
#define _MSC_VER  1900
#endif


//以下的编译器支持
#ifndef _Out_writes_bytes_
#define _Out_writes_bytes_(x)  _Out_
#endif

#ifndef _Out_writes_bytes_to_
#define _Out_writes_bytes_to_(a,b) _Out_
#endif

#ifndef _In_reads_bytes_
#define _In_reads_bytes_(x) _In_
#endif

#ifndef _Out_writes_bytes_opt_
#define _Out_writes_bytes_opt_(x) _Out_
#endif
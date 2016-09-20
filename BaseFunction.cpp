#pragma once

#include "BaseFunction.h"
#include "StringHelper.h"
#include <ntddvol.h>
#include "Guard.h"
#include <winscard.h>
#include <atltime.h>
#include "StringI.h"
#include <shellapi.h>
#include <imapi2fs.h>
#include <WinIoCtl.h>
#include <aclapi.h>
#include <wincrypt.h>
#pragma comment(lib,"Crypt32.lib")
#include "unzip.h"
#ifndef USE_CBS_ZIP
#include "unzip.cpp"
#endif
#include "Base.cpp"

#include "FileHelper.cpp"
#include "WofHelper.cpp"
#include "Hook.cpp"
#include "RegHelper.cpp"
#include "Power.cpp"
#include "IniHelper.cpp"
#include "XMLHelper.cpp"
#include "WinInetHelper.cpp"
#include "StreamEx.cpp"
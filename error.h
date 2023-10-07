#ifndef _GODZILLA_ERROR_H_
#define _GODZILLA_ERROR_H_

#include "config.h"

#ifdef _WINDOWS_SYSTEM_
#define GetErrCode GetLastError
#define SetErrCode SetLastError
#endif

#ifdef _LINUX_SYSTEM_
#define GetErrCode() errno
#define SetErrCode(errcode) errno = errcode
#endif


enum class GErrCode
{
	NoError,
	InvalidParameter,	//无效参数

	FileFormatError,	//文件格式错误
	FileNotFound,		//文件未找到
	PathError,			//路径错误
	FileSizeError,		//文件大小错误

	AsmError,			//汇编代码错误
	DisasmError,		//反汇编错误

	AccessError,		//权限错误

	UnknownError,		//未知错误

};


#endif
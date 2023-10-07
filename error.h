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
	InvalidParameter,	//��Ч����

	FileFormatError,	//�ļ���ʽ����
	FileNotFound,		//�ļ�δ�ҵ�
	PathError,			//·������
	FileSizeError,		//�ļ���С����

	AsmError,			//���������
	DisasmError,		//��������

	AccessError,		//Ȩ�޴���

	UnknownError,		//δ֪����

};


#endif
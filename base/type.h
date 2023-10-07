#ifndef _GODZILLA_BASE_GTYPE_H_
#define _GODZILLA_BASE_GTYPE_H_

#include <vector>
#include <string>

#include "config.h"
#include "ghandle.h"


#ifdef _WINDOWS_SYSTEM_
#include <Windows.h>

typedef DWORD64 Fword;
typedef DWORD Dword;
typedef WORD Word;
typedef BYTE Byte;

typedef DWORD pid_t;

#endif

#ifdef _LINUX_SYSTEM_
#include <sys/types.h>

typedef uint64_t Fword;
typedef uint32_t Dword;
typedef uint16_t Word;
typedef uint8_t Byte;

#endif


typedef unsigned long ulong;

#ifdef _ARCH_86_
	typedef Dword Gauge;
#endif


#ifdef _ARCH_64_
	typedef Fword Gauge;
#endif

#define GaugeCast(value)\
	reinterpret_cast<Gauge>(value)

#define FwordCast(value)\
	reinterpret_cast<Fword>(value)

#define DwordCast(value)\
	reinterpret_cast<Dword>(value)

#define VoidPtrCast(value)\
	reinterpret_cast<void*>(value)

#endif
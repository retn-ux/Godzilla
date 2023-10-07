#ifndef _GODZILLA_SYSTEM_WINSYSTEMEXP_H_
#define _GODZILLA_SYSTEM_WINSYSTEMEXP_H_

#include <string>

#include "godzilla.h"

#include "process/gprocess.h"

#include "winsystemdefs.h"

class GProcess;

#ifdef _WINDOWS_SYSTEM_

#define NTSTATUS_SUCCESS BCRYPT_SUCCESS
#define WS_NTDLL_NAME L"ntdll.dll"

typedef NTSTATUS
(CALLBACK* PfnNtQueryInformatioProcess)(
	IN HANDLE,
	IN PROCESSINFOCLASS,
	OUT PVOID,
	IN ULONG,
	OUT PULONG
	);

typedef NTSTATUS
(WINAPI* PfnRtlCreateProcess) (
	IN HANDLE,
	IN PSECURITY_DESCRIPTOR,
	IN BOOLEAN,
	IN ULONG,
	IN OUT PULONG,
	IN OUT PULONG,
	IN PVOID,
	IN PVOID,
	OUT PHANDLE,
	OUT PCLIENT_ID
	);

typedef NTSTATUS
(WINAPI* PfnZwTerminateThread)(
	HANDLE,
	ULONG
	);

typedef __kernel_entry NTSTATUS
(NTAPI* PfnNtQueryInformationThread)(
	IN HANDLE,
	IN THREADINFOCLASS,
	OUT PVOID,
	IN ULONG,
	OUT PULONG
	);

typedef __kernel_entry NTSTATUS
(NTAPI* PfnNtQuerySystemInformation)(
	IN SYSTEM_INFORMATION_CLASS,
	OUT PVOID,
	IN ULONG,
	OUT PULONG
	);

typedef NTSTATUS
(NTAPI* PfnRtlAdjustPrivilege)(
	IN ULONG Privilege,
	IN BOOLEAN Enable,
	IN BOOLEAN CurrentThread,
	OUT PBOOLEAN Enabled
	);

typedef NTSTATUS
(NTAPI* PfnZwShutdownSystem)(
	IN SHUTDOWN_ACTION action
	);

typedef void
(NTAPI* PfnNtRaiseHardError)(
	IN NTSTATUS,
	IN ULONG,
	IN PUNICODE_STRING,
	IN PVOID,
	IN HARDERROR_RESPONSE_OPTION,
	OUT PHARDERROR_RESPONSE
	);

bool NtQueryInformationProcess(
	GProcess* process,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID           ProcessInformation,
	IN ULONG            ProcessInformationLength,
	OUT PULONG          ReturnLength);

#endif

#endif
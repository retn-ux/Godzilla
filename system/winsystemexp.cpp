#include "winsystemexp.h"

#ifdef _WINDOWS_SYSTEM_


bool NtQueryInformationProcess(
	GProcess* process,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID           ProcessInformation,
	IN ULONG            ProcessInformationLength,
	OUT PULONG          ReturnLength) 
{
	PfnNtQueryInformatioProcess fun_ptr =
		(PfnNtQueryInformatioProcess)process->GetFuncAddress(
			WS_NTDLL_NAME,
			"NtQueryInformationProcess"
		);

	if (fun_ptr == nullptr)
		return false;

	if (!NTSTATUS_SUCCESS(fun_ptr(
		process->Handle().GetElement(),
		ProcessInformationClass,
		ProcessInformation,
		ProcessInformationLength,
		ReturnLength))) {
		return false;

	}

	return true;
}

#endif
#include "gprocmemory.h"

bool GProcMemory::IsHavePageType(
	const PageType& type,
	const PageType& have_type
) {
#ifdef _WINDOWS_SYSTEM_
	return static_cast<DWORD>(type) & static_cast<DWORD>(have_type);
#endif
}

GProcMemory::GProcMemory():hd_() {}

GProcMemory::GProcMemory(const GHandle& hd):hd_(hd) {}

GProcMemory::GProcMemory(GHandle&& hd) :hd_(std::forward<GHandle>(hd)) {}

GProcMemory::GProcMemory(const GProcMemory& other) :hd_(other.hd_) {}

GProcMemory::GProcMemory(GProcMemory&& other) :hd_(std::forward<GHandle>(other.hd_)) {}

GProcMemory::~GProcMemory() {}


void GProcMemory::SetHandle(const GHandle& hd) {
	hd_ = hd;
}

GErrCode GProcMemory::Read(
	Gauge off,
	void* buffer,
	size_t size,
	size_t* numberOfBytesRead
) {
	if (off == 0 || hd_.IsInvalid())
		return GErrCode::InvalidParameter;
#ifdef _WINDOWS_SYSTEM_
	SIZE_T rs = 0;
	if (!ReadProcessMemory(
		hd_.GetElement(),
		VoidPtrCast(off),
		buffer,
		size,
		numberOfBytesRead)
		)
		return GErrCode::UnknownError;
#endif
	return GErrCode::NoError;
}

GErrCode GProcMemory::Write(
	Gauge addr,
	const void* const buffer,
	size_t size,
	size_t* numberOfBytesWritten
) {
	if (addr == 0 || hd_.IsInvalid())
		return GErrCode::InvalidParameter;
#ifdef _WINDOWS_SYSTEM_
	if (!WriteProcessMemory(
		hd_.GetElement(),
		VoidPtrCast(addr),
		buffer,
		size,
		numberOfBytesWritten)
	)
		return GErrCode::UnknownError;
#endif
	return GErrCode::NoError;
}

Gauge GProcMemory::Alloc(
	Gauge address,
	size_t size,
	PageType type
) {
	if (hd_.IsInvalid())
		return 0;

#ifdef _WINDOWS_SYSTEM_
	return GaugeCast(VirtualAllocEx(
		hd_.GetElement(),
		VoidPtrCast(address),
		size,
		MEM_COMMIT | MEM_RESERVE,
		(DWORD)type
	));
#endif
}


bool GProcMemory::Free(Gauge address) {
	if (address == 0 || hd_.IsInvalid())
		return false;

#ifdef _WINDOWS_SYSTEM_
	if (!VirtualFreeEx(
		hd_.GetElement(),
		VoidPtrCast(address),
		0,
		MEM_RELEASE
	))
		return false;
#endif
	return true;
}

bool GProcMemory::GetPageType(
	Gauge address,
	 GProcMemory::PageType* type
) {
	if(address==0)return GErrCode::InvalidParameter
#ifdef _WINDOWS_SYSTEM_
	MEMORY_BASIC_INFORMATION memoryBaseInfo;
	if (!VirtualQueryEx(
		hd_.GetElement(),
		VoidPtrCast(address),
		&memoryBaseInfo,
		sizeof(memoryBaseInfo)
		
		)) 
	{
		return false;
	}
	*type = static_cast<PageType>(memoryBaseInfo.Protect);
#endif
	return true;
}

Gauge GProcMemory::GetBaseAddress(Gauge addr) {
	if (addr == 0)return 0;
	Gauge baseAddress = 0;
#ifdef _WINDOWS_SYSTEM_
	MEMORY_BASIC_INFORMATION memoryBaseInfo;
	if (!VirtualQueryEx(
		hd_.GetElement(),
		VoidPtrCast(addr),
		&memoryBaseInfo,
		sizeof(memoryBaseInfo)

	))
	{
		return 0;
	}
	baseAddress = GaugeCast(memoryBaseInfo.AllocationBase);
#endif
	return baseAddress;
}

bool GProcMemory::ChangePageType(
	Gauge address,
	size_t size,
	PageType type,
	PageType* old_type
) {
#ifdef _WINDOWS_SYSTEM_
	if (!VirtualProtectEx(
		hd_.GetElement(),
		VoidPtrCast(address),
		size,
		(DWORD)(type),
		(Dword*)old_type
	)) return false;

#endif
	return true;
}
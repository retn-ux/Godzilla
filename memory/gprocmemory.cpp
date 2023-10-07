#include "gprocmemory.h"

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
	SIZE_T ws = 0;

	if (!WriteProcessMemory(
		hd_.GetElement(),
		VoidPtrCast(addr),
		buffer,
		size,
		numberOfBytesWritten)
	)
		return GErrCode::UnknownError;

	if (ws != size)return GErrCode::UnknownError;
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


bool GProcMemory::Free(pins_t ins, Gauge address) {
	if (address == 0 || hd_.IsInvalid())
		return false;

#ifdef _WINDOWS_SYSTEM_
	if (!VirtualFreeEx(
		ins,
		VoidPtrCast(address),
		0,
		MEM_RELEASE
	))
		return false;
#endif
	return true;
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
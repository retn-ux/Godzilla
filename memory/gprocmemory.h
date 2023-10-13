#ifndef _GODZILLA_MEMORY_GPROCMEMORY_H_
#define _GODZILLA_MEMORY_GPROCMEMORY_H_

#include "godzilla.h"
#include "interface/irwriter.h"

class GProcMemory:public IRWriter {
public:
	enum class PageType {
#ifdef _WINDOWS_SYSTEM_
		Read = PAGE_READONLY,
		ReadWrite = PAGE_READWRITE,
		ReadWriteExecute = PAGE_EXECUTE_READWRITE,
		Execute = PAGE_EXECUTE,
		ReadExecute = PAGE_EXECUTE_READ,
#endif
	};

	static bool IsHavePageType(
		const PageType& type,
		const PageType& have_type
	);
public:
	GProcMemory();
	GProcMemory(const GHandle& hd);
	GProcMemory(GHandle&& hd);
	GProcMemory(const GProcMemory& other);
	GProcMemory(GProcMemory&& other);
	~GProcMemory();
public:
	void SetHandle(const GHandle& hd);
	GErrCode Read(
		Gauge off,
		void* buffer,
		size_t size,
		size_t* numberOfBytesRead
	) override;

	GErrCode Write(
		Gauge addr,
		const void* const buffer,
		size_t size,
		size_t* numberOfBytesWritten
	) override;

	Gauge Alloc(
		Gauge address,
		size_t size,
		PageType type
	);

	Gauge GetBaseAddress(Gauge addr);

	bool Free(Gauge address);

	bool GetPageType(
		Gauge address,
		PageType* type
	);

	bool ChangePageType(
		Gauge address,
		size_t size,
		PageType type,
		PageType* old_type
	);
private:
	GHandle hd_;
};

#endif
#ifndef _GODZILLA_HOOK_GINTERCEPTOR_H_
#define _GODZILLA_HOOK_GINTERCEPTOR_H_
#include <string>
#include <vector>

#include "godzilla.h"
#include "common/common.h"
#include "memory/gprocmemory.h"
#include "interface/iasmengine.h"

typedef void (__stdcall *DetourFuncPtr)(Gauge esp, Gauge ebp);

class ISpringboard {
public:
	ISpringboard() {}
	virtual ~ISpringboard() {}
public:
	virtual std::vector<Byte> GetSpringboardCode(
		IAssembler& assembler,
		Gauge addr
	) = 0;
};

//push只可以跳转4个字节
class GPushRetSpringboard:public ISpringboard {
public:
	GPushRetSpringboard();
	~GPushRetSpringboard();
public:
	std::vector<Byte> GetSpringboardCode(
		IAssembler& assembler,
		Gauge addr
	) override;
};

class GJmpEaxSpringboard :public ISpringboard {
public:
	GJmpEaxSpringboard();
	~GJmpEaxSpringboard();
public:
	std::vector<Byte> GetSpringboardCode(
		IAssembler& assembler,
		Gauge addr
	) override;
};

class GInterceptor {
public:
	struct Chunk {
		Gauge targetAddr;
		std::vector<Byte> orginalCode;
		Gauge detourAddr;
		Gauge detourSize;
	};
public:
	GInterceptor();
	~GInterceptor();
public:
	GErrCode CreateBaseHook(
		GProcMemory& memory,
		IDisassembler& engine,
		Gauge addr,
		std::vector<Byte> springboard_code
	);

	//code要写入的字节码
	GErrCode CreateByteCodeHook(
		GProcMemory& memory,
		IAsmEngine& engine,
		Gauge addr,
		ISpringboard& springboard,
		std::vector<Byte> code
	);

	GErrCode CreateFuncHook(
		GProcMemory& memory,
		IAsmEngine& engine,
		Gauge addr,
		ISpringboard& springboard,
		DetourFuncPtr func
	);

	//被hook的地址
	GErrCode Release(GProcMemory& memory,Gauge addr);
	GErrCode Close(GProcMemory& memory);
private:
	static GErrCode CreateBaseHook(
		GProcMemory& memory,
		IDisassembler& engine,
		Gauge addr,
		std::vector<Byte> springboard_code,
		Chunk* chunk
	);

	static GErrCode CreateByteCodeHook(
		GProcMemory& memory,
		IAsmEngine& engine,
		Gauge addr,
		ISpringboard& springboard,
		std::vector<Byte> code,
		Chunk* chunk
	);
private:
	std::vector<Chunk> chunk_list_;
};

std::vector<Byte> GetDetourCode(
	IAsmEngine& engine,
	Gauge detourcode_addr,
	Gauge func_addr,
	Gauge hooked_addr,
	std::vector<Byte> byte_code
);
#endif
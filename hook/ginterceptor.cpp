#include "ginterceptor.h"

GPushRetSpringboard::GPushRetSpringboard() {

}

GPushRetSpringboard::~GPushRetSpringboard() {

}

std::vector<Byte> GPushRetSpringboard::GetSpringboardCode(
	IAssembler& engine,
	Gauge addr
) {
	std::string codestr = "push 0x" + ConvertToHexStr(addr)+"\nret";
	return std::move(engine.GetAsmCode(0, codestr));
}

GJmpEaxSpringboard::GJmpEaxSpringboard() {}
GJmpEaxSpringboard::~GJmpEaxSpringboard() {}
std::vector<Byte> GJmpEaxSpringboard::GetSpringboardCode(
	IAssembler& assembler,
	Gauge addr
) {
#ifdef _ARCH_X86_
	std::string codestr = "mov eax,0x" + ConvertToHexStr(addr) + "\ncall eax";
#endif
#ifdef _ARCH_64_
	std::string codestr = "mov rax,0x" + ConvertToHexStr(addr) + "\ncall rax";
#endif
	return std::move(assembler.GetAsmCode(0, codestr));
}

GInterceptor::GInterceptor() {

}

GInterceptor::~GInterceptor() {}

GErrCode GInterceptor::CreateBaseHook(
	GProcMemory& memory,
	IDisassembler& engine,
	Gauge addr,
	std::vector<Byte> springboard_code,
	Chunk* chunk
) {
	if (chunk == nullptr || springboard_code.empty())
		return GErrCode::InvalidParameter;
	if (addr == 0)return GErrCode::InvalidParameter;
	chunk->targetAddr = addr;
	GErrCode ec = GErrCode::NoError;

	GProcMemory::PageType type;
	
	if (!memory.GetPageType(addr, &type)) return GErrCode::AccessError;

	if (!GProcMemory::IsHavePageType(
		type,
		GProcMemory::PageType::ReadWrite
	)) {
		if (!memory.ChangePageType(
			addr,
			springboard_code.size(),
			GProcMemory::PageType::ReadWrite,
			&type
		)) return GErrCode::AccessError;
	}
	size_t will_read_size = springboard_code.size() + 0x10;
	chunk->orginalCode.resize(will_read_size);
	size_t read_size = 0;
	ec = memory.Read(
		addr,
		chunk->orginalCode.data(),
		will_read_size,
		&read_size
	);
	if (ec != GErrCode::NoError || read_size != will_read_size)
		return GErrCode::UnknownError;

	size_t except_size = engine.LengthDisasm(
		chunk->orginalCode,
		springboard_code.size()
	);
	if (except_size == 0)return GErrCode::DisasmError;
	chunk->orginalCode.erase(
		chunk->orginalCode.begin()+except_size,
		chunk->orginalCode.end()
		);
	
	size_t write_size = 0;
	ec = memory.Write(
		addr,
		springboard_code.data(),
		springboard_code.size(),
		&write_size
	);

	if (ec != GErrCode::NoError || write_size != springboard_code.size())
		return ec;

	if (!memory.ChangePageType(
		addr,
		springboard_code.size(),
		type,
		&type
	)) return GErrCode::UnknownError;
	return GErrCode::NoError;
}

GErrCode GInterceptor::CreateByteCodeHook(
	GProcMemory& memory,
	IAsmEngine& engine,
	Gauge addr,
	ISpringboard& springboard,
	std::vector<Byte> code,
	Chunk* chunk
) {
	if (addr == 0 || chunk == nullptr || code.empty()) 
		return GErrCode::InvalidParameter;
	Gauge byteCodeAddr = memory.Alloc(
		0,
		code.size(),
		GProcMemory::PageType::ReadWriteExecute
	);
	if (byteCodeAddr == 0)return GErrCode::UnknownError;
	chunk->detourAddr = byteCodeAddr;
	chunk->detourSize = code.size();
	auto sbcode = 
		springboard.GetSpringboardCode(engine, byteCodeAddr);
	if (sbcode.empty()) return GErrCode::UnknownError;
	GErrCode ec = CreateBaseHook(
		memory,
		engine,
		addr,
		sbcode,
		chunk
	);
	if (ec != GErrCode::NoError) {
		memory.Free(byteCodeAddr);
		return ec;
	}
	size_t read_size = 0;
	ec = memory.Write(
		byteCodeAddr,
		code.data(),
		chunk->detourSize,
		&read_size
	);
	if (read_size != code.size()) {
		memory.Free(byteCodeAddr);
		return GErrCode::UnknownError;
	}
	if (ec != GErrCode::NoError) {
		memory.Free(byteCodeAddr);
		return ec;
	}
	return ec;
}

GErrCode GInterceptor::CreateBaseHook(
	GProcMemory& memory,
	IDisassembler& engine,
	Gauge addr,
	std::vector<Byte> springboard_code
) {
	Chunk chunk;
	chunk.detourSize = 0;
	GErrCode ec = CreateBaseHook(
		memory,
		engine,
		addr,
		springboard_code,
		&chunk
	);
	if (ec != GErrCode::NoError)return ec;
	chunk_list_.emplace_back(std::move(chunk));
	return ec;
}


GErrCode GInterceptor::CreateByteCodeHook(
	GProcMemory& memory,
	IAsmEngine& engine,
	Gauge addr,
	ISpringboard& springboard,
	std::vector<Byte> code
) {
	Chunk chunk;
	GErrCode ec = CreateByteCodeHook(
		memory,
		engine,
		addr,
		springboard,
		code,
		&chunk
	);
	if (ec != GErrCode::NoError)return ec;
	chunk_list_.emplace_back(std::move(chunk));
	return ec;
}

GErrCode GInterceptor::CreateFuncHook(
	GProcMemory& memory,
	IAsmEngine& engine,
	Gauge addr,
	ISpringboard& springboard,
	DetourFuncPtr func
) {
	if (func == nullptr || addr == 0)return GErrCode::InvalidParameter;
	Chunk chunk;
	const Gauge ByteCodeSize = 0x100;
	Gauge byteCodeAddr = memory.Alloc(
		0,
		ByteCodeSize,
		GProcMemory::PageType::ReadWriteExecute
	);
	if (byteCodeAddr == 0)return GErrCode::AccessError;
	chunk.detourAddr = byteCodeAddr;
	chunk.detourSize = ByteCodeSize;
	auto sbcode = 
		springboard.GetSpringboardCode(engine, byteCodeAddr);
	GErrCode ec = CreateBaseHook(
		memory,
		engine,
		addr,
		sbcode,
		&chunk
	);
	if (ec != GErrCode::NoError) {
		memory.Free(byteCodeAddr);
		return ec;
	}

	auto code = GetDetourCode(
		engine,
		byteCodeAddr,
		GaugeCast(func),
		addr,
		chunk.orginalCode
	);
	if (code.empty())return GErrCode::UnknownError;
	size_t write_size = 0;
	ec = memory.Write(
		byteCodeAddr,
		code.data(),
		code.size(),
		&write_size
	);
	if (ec != GErrCode::NoError) {
		memory.Free(byteCodeAddr);
		return ec;
	}

	if (write_size != code.size()) {
		memory.Free(byteCodeAddr);
		return GErrCode::UnknownError;
	}
	

	return ec;
}


//��hook�ĵ�ַ
GErrCode GInterceptor::Release(GProcMemory& memory, Gauge addr) {
	return GErrCode::NoError;
}

GErrCode GInterceptor::Close(GProcMemory& memory) {
	return GErrCode::NoError;
}


std::vector<Byte> GetDetourCode(
	IAsmEngine& engine, 
	Gauge detourcode_addr,
	Gauge func_addr,
	Gauge hooked_addr,
	std::vector<Byte> byte_code
) 
{
	std::string opcode;
#ifdef _ARCH_86_
	opcode += "pushfd\n";
	opcode += "pushad\n";
	opcode += "push esp";
	opcode += "push ebp";
	opcode += "mov eax,0x";
	opcode += ConvertToHexStr(func_addr);
	opcode += "\ncall eax\n";
	opcode += "pop ebp";
	opcode += "pop esp";
	opcode += "popad\n";
	opcode += "popfd\n";
#endif

#ifdef _ARCH_64_
	opcode += "push rax\n";
	opcode += "push rcx\n";
	opcode += "push rdx\n";
	opcode += "push rbx\n";
	opcode += "push rsp\n";
	opcode += "push rbp\n";
	opcode += "push rsi\n";
	opcode += "push rdi\n";
	opcode += "push r8\n";
	opcode += "push r9\n";
	opcode += "push r10\n";
	opcode += "push r11\n";
	opcode += "push r12\n";
	opcode += "push r13\n";
	opcode += "push r14\n";
	opcode += "push r15\n";
	opcode += "mov rcx,rsp\n";
	opcode += "mov rdx,rbp\n";
	opcode += "mov rax,0x";
	opcode += ConvertToHexStr(func_addr);
	opcode += "\ncall rax\n";

	opcode += "pop r15\n";
	opcode += "pop r14\n";
	opcode += "pop r13\n";
	opcode += "pop r12\n";
	opcode += "pop r11\n";
	opcode += "pop r10\n";
	opcode += "pop r9\n";
	opcode += "pop r8\n";
	opcode += "pop rdi\n";
	opcode += "pop rsi\n";
	opcode += "pop rbp\n";
	opcode += "pop rsp\n";
	opcode += "pop rbx\n";
	opcode += "pop rdx\n";
	opcode += "pop rcx\n";
	opcode += "pop rax\n";	
#endif

	auto opstr_list = 
		engine.GetDisasmCode(hooked_addr, byte_code);
	for (auto& it : opstr_list) {
		opcode += it;
		opcode += "\n";
	}

	opcode += "mov rax,0x";
	opcode += ConvertToHexStr(hooked_addr + byte_code.size());
	opcode += "\npush rax\n";
	opcode += "ret";
	auto result = engine.GetAsmCode(detourcode_addr, opcode);
	return result;
}
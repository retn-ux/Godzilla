#ifndef _GODZILLA_ASM_GASMENGINE_H_
#define _GODZILLA_ASM_GASMENGINE_H_

#include "interface/iasmengine.h"
#include "gassembler.h"
#include "gdisassembler.h"


class GAsmEngine:public IAsmEngine {
public:
	GAsmEngine();
	GAsmEngine(
		IAssemblerMode& asm_mode,
		IDisassemblerMode& disasm_mode
	);
	GAsmEngine(const GAsmEngine& other);
	GAsmEngine(GAsmEngine&& other);
public:
	void SetMode(
		IAssemblerMode& asm_mode,
		IDisassemblerMode& disasm_mode
	);

	std::vector<std::string> GetDisasmCode(
		Fword baseAddress,
		const std::vector<Byte>& buffer
	) override;

	size_t LengthDisasm(
		const std::vector<Byte>& buffer,
		size_t expected_size
	) override;

	std::vector<Byte> GetAsmCode(
		Fword baseAddress,
		const std::string& opcode
	) override;
private:
	GAssembler assembler_;
	GDisassembler disassembler_;
};


#endif
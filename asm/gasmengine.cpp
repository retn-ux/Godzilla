#include "gasmengine.h"


GAsmEngine::GAsmEngine() {

}

GAsmEngine::GAsmEngine(
	IAssemblerMode& asm_mode,
	IDisassemblerMode& disasm_mode
):assembler_(asm_mode),disassembler_(disasm_mode) {

}

GAsmEngine::GAsmEngine(const GAsmEngine& other):
	assembler_(other.assembler_) ,
	disassembler_(other.disassembler_)
{
	
}

GAsmEngine::GAsmEngine(GAsmEngine&& other) :
	assembler_(std::forward<GAssembler>(other.assembler_)),
	disassembler_(std::forward<GDisassembler>(other.disassembler_)) {

}

void GAsmEngine::SetMode(
	IAssemblerMode& asm_mode,
	IDisassemblerMode& disasm_mode
) {
	assembler_.SetMode(asm_mode);
	disassembler_.SetMode(disasm_mode);
}

std::vector<std::string> GAsmEngine::GetDisasmCode(
	Fword baseAddress,
	const std::vector<Byte>& buffer
) {
	return disassembler_.GetDisasmCode(baseAddress, buffer);
}

size_t GAsmEngine::LengthDisasm(
	const std::vector<Byte>& buffer,
	size_t expected_size
) {
	return disassembler_.LengthDisasm(buffer, expected_size);
}

std::vector<Byte> GAsmEngine::GetAsmCode(
	Fword baseAddress,
	const std::string& opcode
) {
	return assembler_.GetAsmCode(baseAddress, opcode);
}
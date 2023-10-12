#include "gdisassembler.h"

const GDisassembler::Engine GDisassembler::InvalidEngine = 0;

GDisassembler::GDisassembler():engine_(InvalidEngine) {

}

GDisassembler::GDisassembler(IDisassemblerMode& mode) :
	engine_(InvalidEngine) 
{
	Open(mode);
}
GDisassembler::GDisassembler(const GDisassembler& other) :
	engine_(InvalidEngine) 
{
	engine_ = other.engine_;
}
GDisassembler::GDisassembler(GDisassembler&& other) {
	engine_ = other.engine_;
	other.engine_ = InvalidEngine;
}
GDisassembler::~GDisassembler() {
	Close();
}

void GDisassembler::SetMode(IDisassemblerMode& mode) {
	Close();
	Open(mode);
}

std::vector<std::string> GDisassembler::GetDisasmCode(
	Fword baseAddress,
	const std::vector<Byte>& buffer
) 
{
	std::vector<std::string> result;
	cs_insn* insn = nullptr;
	size_t count = cs_disasm(
		engine_,
		buffer.data(),
		buffer.size(),
		baseAddress,
		0,
		&insn
	);
	if (count > 0)
	{
		for (size_t i = 0; i < count; ++i) {
			std::string tmp = insn[i].mnemonic;
			tmp = tmp + " " + insn[i].op_str;
			result.emplace_back(std::move(tmp));
		}
		cs_free(insn, count);
		return std::move(result);
	}
	return result;
}

size_t GDisassembler::LengthDisasm(
	const std::vector<Byte>& buffer,
	size_t expected_size
)  
{
	size_t result = 0;
	cs_insn* insn = nullptr;

	size_t count = cs_disasm(
		engine_,
		buffer.data(),
		buffer.size(),
		0,
		0,
		&insn
	);
	if (count > 0)
	{
		for (size_t i = 0; i < count; ++i) {
			result += insn[i].size;
			if (result >= expected_size) {
				cs_free(insn, count);
				return std::move(result);
			}
		}
	}
	return 0;
}

void GDisassembler::Close() {
	if (engine_ != InvalidEngine) {
		cs_close(&engine_);
		engine_ = InvalidEngine;
	}
}

void GDisassembler::Open(IDisassemblerMode& mode) {
	cs_open(mode.GetArch(), mode.GetMode(), &engine_);
}
#include "gassembler.h"

const GAssembler::Engine GAssembler::InvalidEngine = nullptr;

GAssembler::GAssembler():engine_(InvalidEngine) {}

GAssembler::GAssembler(IAssemblerMode& mode) :engine_(InvalidEngine) {
	Open(mode);
}

GAssembler::GAssembler(const GAssembler& other) :engine_(InvalidEngine) {
	engine_ = other.engine_;
}

GAssembler::GAssembler(GAssembler&& other) :engine_(InvalidEngine) {
	engine_ = other.engine_;
	other.engine_ = InvalidEngine;
}

GAssembler::~GAssembler() {
	Close();
}

void GAssembler::Close() {
	if (engine_ != InvalidEngine) {
		ks_close(engine_);
		engine_ = InvalidEngine;
	}
}

void GAssembler::Open(IAssemblerMode& mode) {
	ks_open(mode.GetArch(), mode.GetMode(), &engine_);
}


void GAssembler::SetMode(IAssemblerMode& mode) {
	Close();
	Open(mode);
}

std::vector<Byte> GAssembler::GetAsmCode(
	Fword baseAddress,
	const std::string& opcode
) {
	std::vector<Byte> result;
	Byte* byte_data = nullptr;
	size_t size = 0;
	size_t success_count = 0;
	if (ks_asm(
		engine_,
		opcode.data(),
		baseAddress,
		&byte_data,
		&size,
		&success_count
	) == 0)
	{
		result.resize(size);
		memcpy(result.data(), byte_data, size);
		ks_free(byte_data);

		return std::move(result);
	}
	return result;
}
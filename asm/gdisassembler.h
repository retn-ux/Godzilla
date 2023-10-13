#ifndef _GODZILLA_ASM_GDISASSEMBLER_H_
#define _GODZILLA_ASM_GDISASSEMBLER_H_

#include "godzilla.h"
#include "interface/iasmengine.h"

#include "capstone/capstone.h"

class IDisassemblerMode {
public:
	using Arch = cs_arch;
	using Mode = cs_mode;
public:
	IDisassemblerMode() {}
	virtual ~IDisassemblerMode() {};
public:
	virtual Mode GetMode() = 0;
	virtual Arch GetArch() = 0;
};


class GDisasmX86Arch32Mode:public IDisassemblerMode {
public:
	GDisasmX86Arch32Mode() {}
	~GDisasmX86Arch32Mode() {}
public:

	Arch GetArch() {
		return CS_ARCH_X86;
	}

	Mode GetMode() {
		return CS_MODE_32;
	}
};

class GDisasmX86Arch64Mode :public IDisassemblerMode {
public:
	GDisasmX86Arch64Mode() {}
	~GDisasmX86Arch64Mode() {}
public:

	Arch GetArch() {
		return CS_ARCH_X86;
	}

	Mode GetMode() {
		return CS_MODE_64;
	}
};

class GDisassembler :public IDisassembler {
	using Engine = csh;
	static const Engine InvalidEngine;
public:
	GDisassembler();
	GDisassembler(IDisassemblerMode& mode);
	GDisassembler(const GDisassembler& other);
	GDisassembler(GDisassembler&& other);
	~GDisassembler();

public:
	void SetMode(IDisassemblerMode& mode);
	std::vector<std::string> GetDisasmCode(
		Fword baseAddress,
		const std::vector<Byte>& buffer
	) override;

	size_t LengthDisasm(
		const std::vector<Byte>& buffer,
		size_t expected_size
	) override;
private:
	void Close();
	void Open(IDisassemblerMode& mode);
private:
	Engine engine_;
};

#endif
#ifndef _GODZILLA_ASM_GASSEMBLER_H_
#define _GODZILLA_ASM_GASSEMBLER_H_

#include "godzilla.h"
#include "interface/iasmengine.h"

#include "keystone/keystone.h"

class IAssemblerMode {
public:
	using Arch = ks_arch;
	using Mode = ks_mode;
public:
	IAssemblerMode() {}
	virtual ~IAssemblerMode() {}
public:
	virtual Mode GetMode() = 0;
	virtual Arch GetArch() = 0;
};

class GAsmX86Arch32Mode :public IAssemblerMode {
public:
	GAsmX86Arch32Mode() {}
	~GAsmX86Arch32Mode() {}
public:

	Arch GetArch() override {
		return KS_ARCH_X86;
	}

	Mode GetMode() override {
		return KS_MODE_32;
	}
};

class GAsmX86Arch64Mode :public IAssemblerMode {
public:
	GAsmX86Arch64Mode() {}
	~GAsmX86Arch64Mode() {}
public:

	Arch GetArch() {
		return KS_ARCH_X86;
	}

	Mode GetMode() {
		return KS_MODE_64;
	}
};

class GAssembler:public IAssembler {
private:
	using Engine = ks_engine*;
	static const Engine InvalidEngine;
public:
	GAssembler();
	GAssembler(IAssemblerMode& mode);
	GAssembler(const GAssembler& other);
	GAssembler(GAssembler&& other);
	~GAssembler();
public:
	void SetMode(IAssemblerMode& mode);
	std::vector<Byte> GetAsmCode(
		Fword baseAddress,
		const std::string& opcode
	) override;
private:
	void Close();
	void Open(IAssemblerMode& mode);
private:
	Engine engine_;
};

#endif
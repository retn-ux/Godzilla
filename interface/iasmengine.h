#ifndef _GODZILLA_INTERFACE_IASMENGINE_H_
#define _GODZILLA_INTERFACE_IASMENGINE_H_

#include <vector>
#include <string>

#include "godzilla.h"

class IAssembler {
public:
	IAssembler() {}
	virtual ~IAssembler() {}
public:
	virtual std::vector<Byte> GetAsmCode(
		Fword baseAddress,
		const std::string& opcode
	) = 0;
};

class IDisassmebler {
public:
	IDisassmebler() {}
	virtual ~IDisassmebler() {}
public:
	virtual std::vector<std::string> GetDisasmCode(
		Fword baseAddress,
		const std::vector<Byte>& buffer
	) = 0;

	virtual size_t LengthDisasm(
		const std::vector<Byte>& buffer,
		size_t expected_size
	) = 0;
};

class IAsmEngine :public IAssembler, IDisassmebler {
public:
	IAsmEngine() {}
	virtual ~IAsmEngine() {}
};

#endif
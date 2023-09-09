#ifndef _GODZILLA_INTERFACE_COMMAND_H_
#define _GODZILLA_INTERFACE_COMMAND_H_

#include "godzilla.h"

class IReader {
public:
	IReader() {};

	virtual GErrCode Read(
		Gauge addr,
		void* buffer,
		size_t size
	) = 0;

	virtual ~IReader() {};
};

class IWriter {
public:
	IWriter() {};

	virtual GErrCode Write(
		Gauge addr,
		const void* const buffer,
		size_t size
	) = 0;

	virtual ~IWriter() {};
};

#endif
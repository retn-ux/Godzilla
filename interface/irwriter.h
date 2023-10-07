#ifndef _GODZILLA_INTERFACE_IRWRITER_H_
#define _GODZILLA_INTERFACE_IRWRITER_H_

#include "godzilla.h"

class IReader {
public:
	IReader() {};

	virtual GErrCode Read(
		Gauge addr,
		void* buffer,
		size_t size,
		size_t* numberOfBytesRead
	) = 0;

	virtual ~IReader() {};
};

class IWriter {
public:
	IWriter() {};

	virtual GErrCode Write(
		Gauge addr,
		const void* const buffer,
		size_t size,
		size_t* numberOfBytesWritten
	) = 0;

	virtual ~IWriter() {};
};


class IRWriter :public IReader, IWriter {
public:
	IRWriter() {}
	virtual ~IRWriter() {}
};

#endif
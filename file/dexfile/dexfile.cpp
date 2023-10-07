#include "dexfile.h"


bool IsDexFile(const DexHeader* header) {
	if (!memcmp(&header->magic, DEX_MAGIC_NUM, DEX_MAGIC_LEN)) {
		return true;
	}
	return false;
}




GErrCode GetDexHeader(
	IReader& reader,
	Gauge baseAddr,
	DexHeader* dexheader
) {
	size_t read_size = 0;
	GErrCode result = reader.Read(
		baseAddr,
		dexheader,
		sizeof(DexHeader),
		&read_size);
	if (result != GErrCode::NoError && read_size != sizeof(DexHeader))
	{
		return result;
	}

	if (!IsDexFile(dexheader)) {
		return GErrCode::FileFormatError;
	}
	return GErrCode::NoError;
}

DexFile::DexFile() {}

DexFile::~DexFile() {}

GErrCode DexFile::Init(IReader* reader, Gauge baseAddress) {
	GErrCode ec = GetDexHeader(
		*reader, baseAddress, &header_
	);

	if (ec != GErrCode::NoError)
		return ec;
	return GErrCode::NoError;
}



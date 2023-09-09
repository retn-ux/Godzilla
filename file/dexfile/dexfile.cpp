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
	GErrCode result = reader.Read(
		baseAddr,
		dexheader,
		sizeof(DexHeader));
	if (result != GErrCode::NoError)
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

GErrCode CreateDexMapList(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& dexhdr,
	DexMapList* maplist
) {
	maplist->size = 0;
	GErrCode ec = reader.Read(
		baseAddr + dexhdr.mapOff,
		&maplist->size,
		sizeof(maplist->size));
	if (ec != GErrCode::NoError)
	{
		maplist->size = 0;
		return ec;
	}

	maplist->list = new DexMapItem[maplist->size];
	ec = reader.Read(
		baseAddr + dexhdr.mapOff + sizeof(maplist->size),
		maplist->list,
		sizeof(DexMapItem) * maplist->size);
	if (ec != GErrCode::NoError)
	{
		delete[] maplist->list;
		maplist->size = 0;
		maplist->list = nullptr;
		return ec;
	}
	return ec;
}

void ReleaseDexMapList(
	DexMapList* maplist
) {
	delete[] maplist->list;
	maplist->size = 0;
	maplist->list = nullptr;
}


DexMapItem* FindDexMapItem(
	const DexMapList* maplist,
	MapItemType type
) {
	for (Dword i = 0; i < maplist->size; i++) {
		if (maplist->list[i].type == type) {
			return &maplist->list[i];
		}
	}

	return nullptr;
}



GErrCode CreateDexStringId(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& dexhdr,
	DexStringId** stringId,
	Dword* ret_size
) {
	*ret_size = dexhdr.stringidsSize;
	*stringId = new DexStringId[*ret_size];
	GErrCode ec = reader.Read(
		baseAddr + dexhdr.stringidsOff,
		*stringId,
		dexhdr.stringidsSize * sizeof(DexStringId));
	if (ec != GErrCode::NoError)
	{
		delete[] *stringId;
		*stringId = nullptr;
	}
	return ec;
}

void ReleaseDexStringId(
	DexStringId* strid_list
) {
	delete[] strid_list;
}


void ParseLEB128Byte(Byte value, LEB128Byte* lebByte) {
	lebByte->nextByte = value >> 7;
	lebByte->value = value & 0x7f;
}


int GetValueOfLEB128(Byte* data, size_t size, size_t* byteSize) {
	int result = 0;
	size_t leb128Size = size;
	if (size > 5)
		leb128Size = 5;

	LEB128Byte lebByte;
	for (*byteSize = 0; *byteSize < leb128Size; (*byteSize)++) {
		ParseLEB128Byte(data[*byteSize], &lebByte);
		result |= lebByte.value << (7 * (*byteSize));
		if (!lebByte.nextByte) {
			(*byteSize)++;
			break;
		}
	}

	return result;
}


GErrCode CreateDexString(
	IReader& reader,
	Gauge baseAddr,
	const DexStringId& dexStrId,
	DexString* dexstr
) {
	const int BUFF_SIZE = 5;
	Byte buffer[BUFF_SIZE];
	
	GErrCode ec = reader.Read(
		baseAddr + dexStrId.stringDataOff,
		buffer,
		BUFF_SIZE);
	if (ec != GErrCode::NoError)
	{
		return ec;
	}

	size_t byte_size = 0;
	dexstr->length = GetValueOfLEB128(buffer, BUFF_SIZE, &byte_size);
	if (dexstr->length == 0) {
		return GErrCode::NoError;
	}

	dexstr->str = new char[dexstr->length + 1];
	ec = reader.Read(
		baseAddr + dexStrId.stringDataOff + byte_size,
		dexstr->str,
		dexstr->length + 1
	);
	if (ec != GErrCode::NoError)
	{
		delete[] dexstr->str;
		dexstr->str = nullptr;
		dexstr->length = 0;
	}
	return ec;
}


void ReleaseDexString(DexString* str) {
	delete[] str->str;
	str->str = nullptr;
	str->length = 0;
}


GErrCode CreateDexTypeId(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& dexhdr,
	DexTypeId** typeId,
	Dword* ret_size
) {
	*ret_size = dexhdr.typeidsSize;
	*typeId = new DexTypeId[*ret_size];
	GErrCode ec = reader.Read(
		baseAddr + dexhdr.typeidsOff,
		*typeId,
		dexhdr.typeidsSize * sizeof(DexTypeId));
	if (ec != GErrCode::NoError)
	{
		delete[] *typeId;
		*typeId = nullptr;
	}
	return ec;
}


void ReleaseDexTypeId(DexTypeId* typeids) {
	delete[] typeids;
}


GErrCode CreateDexTypeList(
	IReader& reader,
	Gauge baseAddr,
	const DexProtoId& protoId,
	DexTypeList* typeList
) {
	GErrCode ec = reader.Read(
		baseAddr + protoId.parametersOff,
		&typeList->size,
		sizeof(typeList->size)
	);
	if (ec != GErrCode::NoError)
	{
		typeList->size = 0;
		return ec;
	}
	typeList->list = new DexTypeItem[typeList->size];
	ec = reader.Read(
		baseAddr + protoId.parametersOff + sizeof(typeList->size),
		typeList->list,
		typeList->size * sizeof(DexTypeItem)
	);
	if (ec != GErrCode::NoError)
	{
		delete[] typeList->list;
		typeList->list = nullptr;
		typeList->size = 0;
	}
	return ec;
}


void ReleaseTypeList(DexTypeList* typelist) {
	delete[] typelist->list;
	typelist->list = nullptr;
	typelist->size = 0;
}

GErrCode CreateDexProtoId(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& dexhdr,
	DexProtoId** protoId,
	Dword* ret_size
) {
	*ret_size = dexhdr.protoidsSize;
	*protoId = new DexProtoId[*ret_size];
	GErrCode ec = reader.Read(
		baseAddr + dexhdr.protoidsOff,
		*protoId,
		*ret_size * sizeof(DexProtoId)
	);
	if (ec != GErrCode::NoError)
	{
		delete[] *protoId;
		*protoId = nullptr;
	}
	return ec;
}


void ReleaseDexProtoId(DexProtoId* protoIds) {
	delete[] protoIds;
}


GErrCode CreateDexFieldId(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& header,
	DexFieldId** fieldIds,
	Dword* ret_size
) {
	*ret_size = header.fieldidsSize;
	*fieldIds = new DexFieldId[*ret_size];

	GErrCode ec = reader.Read(
		baseAddr + header.fieldidsOff,
		*fieldIds,
		*ret_size * sizeof(DexFieldId)
	);
	if (ec != GErrCode::NoError)
	{
		delete[] * fieldIds;
		*fieldIds = nullptr;
	}
	return ec;
}


void ReleaseDexFieldId(DexFieldId* fieldIds) {
	delete[] fieldIds;
}


GErrCode CreateDexMethodId(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& header,
	DexMethodId** methodIds,
	Dword* ret_size
) {
	*ret_size = header.methodidsSize;
	*methodIds = new DexMethodId[*ret_size];

	GErrCode ec = reader.Read(
		baseAddr + header.methodidsOff,
		*methodIds,
		*ret_size * sizeof(DexMethodId)
	);
	if (ec != GErrCode::NoError)
	{
		delete[] * methodIds;
		*methodIds = nullptr;
	}
	return ec;
}


void ReleaseDexMethodId(DexMethodId* methodIds) {
	delete[] methodIds;
}


GErrCode CreateDexClassDef(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& header,
	DexClassDef** classDefs,
	Dword* retSize
) {
	*retSize = header.classDefsSize;
	*classDefs = new DexClassDef[*retSize];
	GErrCode ec = reader.Read(
		baseAddr + header.classDefsOff,
		*classDefs,
		*retSize
	);

	if (ec != GErrCode::NoError) {
		delete[] *classDefs;
		*classDefs = nullptr;
	}

	return ec;
}


void ReleaseDexClassDef(DexClassDef* classDefs) {
	delete[] classDefs;
}
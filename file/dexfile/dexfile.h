#ifndef _GODZILLA_FILE_DEXFILE_H_
#define _GODZILLA_FILE_DEXFILE_H_

#include "godzilla.h"
#include "common/interface.h"

#define KSHAlDIGESTLEN 20

#define DEX_MAGIC_LEN 8

#define DEX_MAGIC_NUM "dex\n035"

enum MapItemType : uint16_t {
	kDexTypeHeaderItem = 0x0000,
	kDexTypeStringIdItem = 0x0001,
	kDexTypeTypeIdItem = 0x0002,
	kDexTypeProtoIdItem = 0x0003,
	kDexTypeFieldIdItem = 0x0004,
	kDexTypeMethodIdItem = 0x0005,
	kDexTypeClassDefItem = 0x0006,
	kDexTypeCallSiteIdItem = 0x0007,
	kDexTypeMethodHandleItem = 0x0008,
	kDexTypeMapList = 0x1000,
	kDexTypeTypeList = 0x1001,
	kDexTypeAnnotationSetRefList = 0x1002,
	kDexTypeAnnotationSetItem = 0x1003,
	kDexTypeClassDataItem = 0x2000,
	kDexTypeCodeItem = 0x2001,
	kDexTypeStringDataItem = 0x2002,
	kDexTypeDebugInfoItem = 0x2003,
	kDexTypeAnnotationItem = 0x2004,
	kDexTypeEncodedArrayItem = 0x2005,
	kDexTypeAnnotationsDirectoryItem = 0x2006,
	kDexTypeHiddenapiClassData = 0xF000,
};


enum {
	ACC_PUBLIC = 0x00000001,       // class, field, method, ic
	ACC_PRIVATE = 0x00000002,       // field, method, ic
	ACC_PROTECTED = 0x00000004,       // field, method, ic
	ACC_STATIC = 0x00000008,       // field, method, ic
	ACC_FINAL = 0x00000010,       // class, field, method, ic
	ACC_SYNCHRONIZED = 0x00000020,       // method (only allowed on natives)
	ACC_SUPER = 0x00000020,       // class (not used in Dalvik)
	ACC_VOLATILE = 0x00000040,       // field
	ACC_BRIDGE = 0x00000040,       // method (1.5)
	ACC_TRANSIENT = 0x00000080,       // field
	ACC_VARARGS = 0x00000080,       // method (1.5)
	ACC_NATIVE = 0x00000100,       // method
	ACC_INTERFACE = 0x00000200,       // class, ic
	ACC_ABSTRACT = 0x00000400,       // class, method, ic
	ACC_STRICT = 0x00000800,       // method
	ACC_SYNTHETIC = 0x00001000,       // field, method, ic
	ACC_ANNOTATION = 0x00002000,       // class, ic (1.5)
	ACC_ENUM = 0x00004000,       // class, field, ic (1.5)
	ACC_CONSTRUCTOR = 0x00010000,       // method (Dalvik only)
	ACC_DECLARED_SYNCHRONIZED =
	0x00020000,       // method (Dalvik only)
	ACC_CLASS_MASK =
	(ACC_PUBLIC | ACC_FINAL | ACC_INTERFACE | ACC_ABSTRACT
		| ACC_SYNTHETIC | ACC_ANNOTATION | ACC_ENUM),
	ACC_INNER_CLASS_MASK =
	(ACC_CLASS_MASK | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC),
	ACC_FIELD_MASK =
	(ACC_PUBLIC | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC | ACC_FINAL
		| ACC_VOLATILE | ACC_TRANSIENT | ACC_SYNTHETIC | ACC_ENUM),
	ACC_METHOD_MASK =
	(ACC_PUBLIC | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC | ACC_FINAL
		| ACC_SYNCHRONIZED | ACC_BRIDGE | ACC_VARARGS | ACC_NATIVE
		| ACC_ABSTRACT | ACC_STRICT | ACC_SYNTHETIC | ACC_CONSTRUCTOR
		| ACC_DECLARED_SYNCHRONIZED),
};

struct DexHeader {
	Byte magic[DEX_MAGIC_LEN]; /* DEX�汾��ʶ */
	Dword checksum; /* adler32���� */
	Byte signature[KSHAlDIGESTLEN]; /* SHA-1ɢ��ֵ */
	Dword fileSize; /* �����ļ��Ĵ�С */
	Dword headerSize; /* DexHeader�ṹ�Ĵ�С */
	Dword endianTag; /* �ֽ����� */
	Dword linkSize; /* ���ӶεĴ�С */
	Dword linkOff; /* ���Ӷε�ƫ���� */
	Dword mapOff; /* DexMaplist���ļ�ƫ�� */
	Dword stringidsSize; /* DexStringid�ĸ��� */
	Dword stringidsOff; /* Dexstringid���ļ�ƫ�� */
	Dword typeidsSize; /* DexTypeid�ĸ��� */
	Dword typeidsOff; /* DexTypeid���ļ�ƫ�� */
	Dword protoidsSize; /* DexProtoid�ĸ��� */
	Dword protoidsOff; /* DexProtoid���ļ�ƫ��*/
	Dword fieldidsSize; /* DexFieldid�ĸ��� */
	Dword fieldidsOff; /* DexFieldid���ļ�ƫ��*/
	Dword methodidsSize; /* DexMethodid�ĸ��� */
	Dword methodidsOff; /* DexMethodid���ļ�ƫ�� */
	Dword classDefsSize; /* DexClassDef�ĸ��� */
	Dword classDefsOff; /* DexClassDef���ļ�ƫ�� */
	Dword dataSize; /* ���ݶεĴ�С */
	Dword dataOff; /* ���ݶε��ļ�ƫ�� */
};

struct DexMapItem {
	Word type;
	Word unused;
	Dword size;
	Dword offset;
};

struct DexStringId {
	Dword stringDataOff;
};

struct LEB128Byte {
	bool nextByte;
	Byte value;
};

struct DexMapList {
	Dword  size;               /* #of entries in list */
	DexMapItem* list;     /* entries */
};

struct DexString {
	int length;
	char* str;
};

struct DexTypeId {
	Dword descriptor_idx;  // index into string_ids
};

struct DexProtoId {
	Dword shortyIdx;
	Dword returnTypeIdx;
	Dword parametersOff;
};

struct DexTypeItem{
	Word typeIdx;
};

struct DexTypeList {
	Dword size;
	DexTypeItem* list;
};

struct DexFieldId {
	Word  classIdx;           /* index into typeIds list for defining class */
	Word  typeIdx;            /* index into typeIds for field type */
	Dword  nameIdx;            /* index into stringIds for field name */
};

struct DexMethodId {
	Word  classIdx;           /* index into typeIds list for defining class */
	Word  protoIdx;           /* index into protoIds for method prototype */
	Dword  nameIdx;            /* index into stringIds for method name */
};

struct DexClassDef {
	Dword  classIdx;           /* index into typeIds for this class */
	Dword  accessFlags;
	Dword  superclassIdx;      /* index into typeIds for superclass */
	Dword  interfacesOff;      /* file offset to DexTypeList */
	Dword  sourceFileIdx;      /* index into stringIds for source file name */
	Dword  annotationsOff;     /* file offset to annotations_directory_item */
	Dword  classDataOff;       /* file offset to class_data_item */
	Dword  staticValuesOff;    /* file offset to DexEncodedArray */
};

struct DexClassDataHeader {
	Dword staticFieldsSize;
	Dword instanceFieldsSize;
	Dword directMethodsSize;
	Dword virtualMethodsSize;
};

struct DexField {
	Dword fieldIdx;    /* index to a field_id_item */
	Dword accessFlags;
};

struct DexMethod {
	Dword methodIdx;    /* index to a method_id_item */
	Dword accessFlags;
	Dword codeOff;      /* file offset to a code_item */
};

struct DexClassData {
	DexClassDataHeader header;
	Gauge staticFields;
	Gauge instanceFields;
	Gauge directMethods;
	Gauge virtualMethods;
};

struct DexCode {
	Word  registersSize;  // �Ĵ�������
	Word  insSize;        // �����ĸ���
	Word  outsSize;       // ������������ʱʹ�õļĴ�������
	Word  triesSize;      // try/catch ������
	Dword  debugInfoOff;   // debug ��Ϣ��ƫ����
	Dword  insnsSize;      // ָ��ĸ���
	Word*  insns;       // ָ�
	/* followed by optional u2 padding */  // 2 �ֽڣ����ڶ���
	/* followed by try_item[triesSize] */
	/* followed by uleb128 handlersSize */
	/* followed by catch_handler_item[handlersSize] */
};


bool IsDexFile(const DexHeader* header);

GErrCode GetDexHeader(
	IReader& reader,
	Gauge baseAddr,
	DexHeader* dexheader
);

class DexFile {
public:
	DexFile();
	~DexFile();
public:
	GErrCode Init(IReader* reader,Gauge baseAddress);
private:
	DexHeader header_;
};


//��Ҫ�ֶ��ͷ�
GErrCode CreateDexMapList(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& dexhdr,
	DexMapList* maplist
);

GErrCode CreateDexStringId(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& dexhdr,
	DexStringId** stringId,
	Dword* ret_size
);

void ReleaseDexStringId(
	DexStringId* strid_list
);

void ReleaseDexMapList(
	DexMapList* maplist
);

DexMapItem* FindDexMapItem(
	const DexMapList* maplist,
	MapItemType type
);

void ParseLEB128Byte(Byte value, LEB128Byte* lebByte);

//0��λΪ���λ
int GetValueOfLEB128(Byte* data, size_t size, size_t* byteSize);

GErrCode CreateDexString(
	IReader& reader,
	Gauge baseAddr,
	const DexStringId& dexStrId,
	DexString* dexstr
);

void ReleaseDexString(DexString* str);

GErrCode CreateDexTypeId(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& dexhdr,
	DexTypeId** typeId,
	Dword* ret_size
);

void ReleaseDexTypeId(DexTypeId* typeids);


GErrCode CreateDexTypeList(
	IReader& reader,
	Gauge baseAddr,
	const DexProtoId& protoId,
	DexTypeList* typeList
);

void ReleaseTypeList(DexTypeList* typelist);

GErrCode CreateDexProtoId(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& dexhdr,
	DexProtoId** protoId,
	Dword* ret_size
);

void ReleaseDexProtoId(DexProtoId* protoIds);

GErrCode CreateDexFieldId(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& header,
	DexFieldId** fieldIds,
	Dword* ret_size
);

void ReleaseDexFieldId(DexFieldId* fieldIds);


GErrCode CreateDexMethodId(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& header,
	DexMethodId** methodIds,
	Dword* ret_size
);

void ReleaseDexMethodId(DexMethodId* methodIds);


GErrCode CreateDexClassDef(
	IReader& reader,
	Gauge baseAddr,
	const DexHeader& header,
	DexClassDef** classDefs,
	Dword* retSize
);

void ReleaseDexClassDef(DexClassDef* classDefs);



#endif
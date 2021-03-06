#include "encoder_impl.h"
#include "file_io.h"
#include "tag_logic.h"
#include "utils.h"

#include<bitset>

const int _DATA_BYTES_32 = 4;
const int _DATA_BYTES_64 = 8;
const int _DATA_BYTES_SAVED_32 = _DATA_BYTES_32 + 1;//ValueSize+tagSize.
const int _DATA_BYTES_SAVED_64 = _DATA_BYTES_64 + 1;//ValueSize+tagSize.
const int _DATA_BYTES_SINGLE = 1;
const int _DATA_BYTES_SAVED_SINGLE = _DATA_BYTES_SINGLE + 1;

using namespace NCData;

bool NCFileIO::EncoderImpl::Encode(Desc* p_descriptor, Byte* src_data)
{
	for (auto field : *p_descriptor)
	{
		if (!EncodeField(field, src_data))
		{
			throw(EncodeException(field));
		}
	}

	return true;
}

bool NCFileIO::EncoderImpl::EncodeField(FieldDesc* p_desc, Byte* src_data)
{
#define NC_CASE_TRANS(_EnumT, _ArrSize, _Func)\
	case(_EnumT):{\
	ByteArray value(_ArrSize);\
	_Func(src_data + p_desc->Offset(), value);\
	WriteBytes(value.bytes, value.use); break;}

	switch (p_desc->Type())
	{
		NC_CASE_TRANS(DataType::INT32, _DATA_BYTES_SAVED_32, IntTrans);
		NC_CASE_TRANS(DataType::INT64, _DATA_BYTES_SAVED_64, IntTrans);
		NC_CASE_TRANS(DataType::UINT32, _DATA_BYTES_SAVED_32, UIntTrans);
		NC_CASE_TRANS(DataType::UINT64, _DATA_BYTES_SAVED_64, UIntTrans);
		NC_CASE_TRANS(DataType::FLOAT, _DATA_BYTES_SAVED_32, FloatTrans);
		NC_CASE_TRANS(DataType::DOUBLE, _DATA_BYTES_SAVED_64, DoubleTrans);
		NC_CASE_TRANS(DataType::BOOL, _DATA_BYTES_SAVED_SINGLE, BoolTrans);
		NC_CASE_TRANS(DataType::CHAR, _DATA_BYTES_SAVED_SINGLE, CharTrans);
	}

	return true;
}

void NCFileIO::EncoderImpl::WriteBytes(Byte* data, int data_bytes)
{
	os.Write(data, data_bytes);
}

void NCFileIO::IntTrans(Byte* src_data, ByteArray& tr_data)
{
	bool bSigned = (tr_data.size == _DATA_BYTES_SAVED_32 ?
		(*((int*)(src_data)) & 0x80000000) :
		(*((int64*)(src_data)) & 0x8000000000000000));

	if (bSigned) SIntTrans(src_data, tr_data);
	else UIntTrans(src_data, tr_data);
}

void NCFileIO::UIntTrans(Byte* src_data, ByteArray& tr_data)
{
	if (tr_data.size == _DATA_BYTES_SAVED_32 ?
		(*((uint*)(src_data)) == 0) :
		(*((uint64*)(src_data)) == 0))//Meet defult value, set with nothing but a tag.
	{
		AddTag(TagType::UNDEFINED, tr_data);
		return;
	}

	// Bytes for int values.
	uint64 val = (tr_data.size == _DATA_BYTES_SAVED_32 ? 
		*((uint*)(src_data)) : 
		*((uint64*)(src_data)));
	if (val >= 0x1000000000000000){
		AddTag(TagType::FIXED64, tr_data);
		BytesTrans(src_data, tr_data);
	}
	else if (val > UINT_MAX){
		AddTag(TagType::VARIANT64, tr_data);
		VariantEncode(val, tr_data);
	}
	else if (val >= 0x10000000){
		AddTag(TagType::FIXED32, tr_data);
		// Will use more bits than variant encoding, save in default.
		BytesTrans(src_data, tr_data);
	}
	else {
		AddTag(TagType::VARIANT32, tr_data);
		VariantEncode(val, tr_data);
	}

}

void NCFileIO::SIntTrans(Byte* src_data, ByteArray& tr_data)
{
	if (tr_data.size == _DATA_BYTES_SAVED_32){
		// First 8-bytes for tag.
		AddTag(TagType::SINT32, tr_data);
		int val = *((int*)(src_data));
		ZigzagEncode(val, tr_data);
	}
	else {
		AddTag(TagType::SINT64, tr_data);
		int64 val = *((int64*)(src_data));
		ZigzagEncode(val, tr_data);
	}
}

void NCFileIO::FloatTrans(Byte* src_data, ByteArray& tr_data)
{
	AddTag(TagType::FIXED32, tr_data);
	BytesTrans(src_data, tr_data);
}

void NCFileIO::DoubleTrans(Byte* src_data, ByteArray& tr_data)
{
	AddTag(TagType::FIXED64, tr_data);
	BytesTrans(src_data, tr_data);
}

void NCFileIO::BoolTrans(Byte* src_data, ByteArray& tr_data)
{
	bool b = *((bool*)(src_data));
	if (!b)// False as default, do not encode.
	{
		AddTag(TagType::UNDEFINED, tr_data);
		return;
	}

	AddTag(TagType::FIXED32, tr_data);
	BytesTrans(src_data, tr_data);
}

void NCFileIO::CharTrans(Byte* src_data, ByteArray& tr_data)
{
	AddTag(TagType::FIXED32, tr_data);
	BytesTrans(src_data, tr_data);
}

void NCFileIO::BytesTrans(Byte* src_data, ByteArray& tr_data)
{
	// Save every bit.
	auto ptr = src_data;
	for (int i = 1; i < tr_data.size; i++)//Without tag(0).
	{
		DebugLog("Code byte [BytesTrans] : ", 
			std::bitset<sizeof(Byte) * 8>(*ptr));
		tr_data.bytes[tr_data.use++] = *(ptr++);
	}
	DebugLog("Coding use bytes : ", tr_data.use);
}

void NCFileIO::VariantEncode(uint64 value, ByteArray& tr_data)
{
	/*
	* Variant encode for uint : value.
	* Each byte saves 7 bits value and 1 bit msb.
	* value is cut form right-side,
	* msb defines if there is next byte data.
	* e.g.	1 000 0001 | 0 000 0001 for 000 0001 000 0001 (129).
	*		1 000 1001 | 0 000 0110 for 000 0110 000 1001 (666).
	*/
	const Byte mask = 0x7F;
	while (value)
	{
		Byte t = (value & mask);
		value >>= 7;
		if (value) t |= 0x80;// msb = 1.
		tr_data.bytes[tr_data.use++] = t;
		DebugLog("Code byte [VariantEncode] : ",
			std::bitset<sizeof(Byte) * 8>(t));
	}
	DebugLog("Coding use bytes : ", tr_data.use);
}

void NCFileIO::ZigzagEncode(int value, ByteArray& tr_data)
{
	/*
	* Zigzag for minus int : value.
	* >>31 : to get sign bit,
	* <<1 : to times 2,
	* ^ : nothing happens when sign bit == 0,
	*	  flipp each bit when sign bit == 1, X-1-2*Compl(abs(value)).
	*	  e.g. sb=0 : abs(value)*2,
	*		   sb=1 : abs(value)*2-1.
	*/
	DebugLog("Zigzag from : ", std::bitset<sizeof(int) * 8>(value));
	int v = (value >> 31) ^ (value << 1);
	DebugLog("To : ", std::bitset<sizeof(int) * 8>(v));
	UIntTrans((Byte*)&v, tr_data);
	//parse:(n>>>1)^ -(n&1).
}

void NCFileIO::ZigzagEncode(int64 value, ByteArray& tr_data)
{
	DebugLog("Zigzag from : ", std::bitset<sizeof(int64) * 8>(value));
	int64 v = (value >> 63) ^ (value << 1);
	DebugLog("To : ", std::bitset<sizeof(int64) * 8>(v));
	UIntTrans((Byte*)&v, tr_data);
}

void NCFileIO::AddTag(TagType type, ByteArray& tr_data)
{
	tr_data.bytes[0] |= CalTagBits(type);
	DebugLog("Add Tag : ", NC_BITSET(tr_data.bytes[0]));
	tr_data.use = (tr_data.use == 0 ? 1 : tr_data.use);
}

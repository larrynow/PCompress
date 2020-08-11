#include "encoder_impl.h"
#include "file_io.h"

#include<bitset>

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
	NC_CASE_TRANS(CPPType::INT32, 4, IntTrans);
	NC_CASE_TRANS(CPPType::INT64, 8, IntTrans);
	NC_CASE_TRANS(CPPType::UINT32, 4, UIntTrans);
	NC_CASE_TRANS(CPPType::UINT64, 8, UIntTrans);
	NC_CASE_TRANS(CPPType::FLOAT, 4, FloatTrans);
	NC_CASE_TRANS(CPPType::DOUBLE, 8, DoubleTrans);
	NC_CASE_TRANS(CPPType::BOOL, 1, BoolTrans);
	NC_CASE_TRANS(CPPType::CHAR, 1, CharTrans);
	}

	return true;
}

void NCFileIO::EncoderImpl::WriteBytes(Byte* data, int data_bytes)
{
	os.Write(data, data_bytes);
}

void NCFileIO::IntTrans(Byte* src_data, ByteArray& tr_data)
{
	bool bSigned = (tr_data.size == 4 ? 
		(*((int*)(src_data)) & 0x80000000) : 
		(*((int64*)(src_data)) & 0x8000000000000000));

	if (bSigned) SIntTrans(src_data, tr_data);
	else UIntTrans(src_data, tr_data);
}

void NCFileIO::UIntTrans(Byte* src_data, ByteArray& tr_data)
{
	// Bytes for int values.
	uint64 val = (tr_data.size == 4 ? *((uint*)(src_data)) : *((uint64*)(src_data)));
	if (val >= 0x1000000000000000){
		AddTag(TagType::FIXED64, tr_data);
		BytesTrans(src_data, tr_data);
	}
	else if (val > UINT_MAX){
		AddTag(TagType::UINT64, tr_data);
		VariantEncode(val, tr_data);
	}
	else if (val >= 0x10000000){
		AddTag(TagType::FIXED32, tr_data);
		// Will use more bits than variant encoding, save in default.
		BytesTrans(src_data, tr_data);
	}
	else {
		AddTag(TagType::UINT32, tr_data);
		VariantEncode(val, tr_data);
	}

}

void NCFileIO::SIntTrans(Byte* src_data, ByteArray& tr_data)
{
	if (tr_data.size == 4){
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
	// First 8-bytes for tag.
	AddTag(TagType::SINT32, tr_data);
	BytesTrans(src_data, tr_data);
}

void NCFileIO::DoubleTrans(Byte* src_data, ByteArray& tr_data)
{
	AddTag(TagType::DOUBLE, tr_data);
	BytesTrans(src_data, tr_data);
}

void NCFileIO::BoolTrans(Byte* src_data, ByteArray& tr_data)
{
	bool b = *((bool*)(src_data));
	if (!b) return;// False as default, DO NOT encode.

	AddTag(TagType::BOOL, tr_data);

	VariantEncode(b, tr_data);
}

void NCFileIO::CharTrans(Byte* src_data, ByteArray& tr_data)
{
	AddTag(TagType::BYTE, tr_data);

	BytesTrans(src_data, tr_data);
}

void NCFileIO::BytesTrans(Byte* src_data, ByteArray& tr_data)
{
	// Only save bits.
	auto ptr = src_data;
	for (int i = 0; i < tr_data.size; i++)
	{
		std::cout << "Code byte [BytesTrans] : " << std::bitset<sizeof(Byte) * 8>(*ptr) << std::endl;
		tr_data.bytes[tr_data.use++] = *(ptr++);
	}
	std::cout << "Coding use bytes : " << tr_data.use << std::endl;
}

void NCFileIO::VariantEncode(uint64 value, ByteArray& tr_data)
{
	const Byte mask = 0x7F;
	while (value)
	{
		Byte t = (value & mask);
		value >>= 7;
		if (value) t |= 0x80;// msb = 1.
		tr_data.bytes[tr_data.use++] = t;
		std::cout << "Code byte [VariantEncode] : " << std::bitset<sizeof(Byte) * 8>(t) << std::endl;
	}
	std::cout << "Coding use bytes : " << tr_data.use << std::endl;
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
	int v = (value >> 31) ^ (value << 1);
	std::cout << "Zigzag to : " << std::bitset<sizeof(int) * 8>(v) << std::endl;
	UIntTrans((Byte*)&v, tr_data);
	//parse:(n>>>1)^ -(n&1).
}

void NCFileIO::ZigzagEncode(int64 value, ByteArray& tr_data)
{
	int64 v = (value >> 63) ^ (value << 1);
	std::cout << "Zigzag to : " << std::bitset<sizeof(int64) * 8>(v) << std::endl;
	UIntTrans((Byte*)&v, tr_data);
}

void NCFileIO::AddTag(TagType type, ByteArray& tr_data)
{

}

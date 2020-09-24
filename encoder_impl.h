#pragma once

#ifndef _NC_ENCODER_IMPL_H_
#define _NC_ENCODER_IMPL_H_

#include"data_desc.h"

namespace NCFileIO
{
	using namespace NCData;

	struct EncodeException : public std::exception
	{
		EncodeException(FieldDesc* _p_field) : pField(_p_field) {}

		virtual const char* what() const override
		{
			return ("Encode Failed at Field [" + pField->FieldName() + "].").c_str();
		}

	private:
		FieldDesc* pField;
	};

	class OutputStream;
	
	class EncoderImpl
	{
	public:
		EncoderImpl(OutputStream& output) : os(output) {}
		//Encode a data into outputstream with a descriptor.
		bool Encode(Desc* p_descriptor, Byte* src_data);
		bool EncodeField(FieldDesc* p_desc, Byte* src_data);

		inline void SaveField(FieldDesc* field, Byte* encoded_data) {};

	private:
		OutputStream& os;

		void WriteBytes(Byte* data, int data_bytes);
	};

	static void IntTrans(Byte* src_data, ByteArray& tr_data);
	static void UIntTrans(Byte* src_data, ByteArray& tr_data);//For positive ints.
	static void SIntTrans(Byte* src_data, ByteArray& tr_data);//For minnor ints.

	inline static void FloatTrans(Byte* src_data, ByteArray& tr_data);//For float.
	inline static void DoubleTrans(Byte* src_data, ByteArray& tr_data);

	static void BoolTrans(Byte* src_data, ByteArray& tr_data);
	static void CharTrans(Byte* src_data, ByteArray& tr_data);

	static void BytesTrans(Byte* src_data, ByteArray& tr_data);
	static void VariantEncode(uint64 value, ByteArray& tr_data);
	inline static void ZigzagEncode(int value, ByteArray& tr_data);
	inline static void ZigzagEncode(int64 value, ByteArray& tr_data);

	inline static void AddTag(TagType type, ByteArray& tr_data);
};

#endif // !_NC_ENCODER_IMPL_H_
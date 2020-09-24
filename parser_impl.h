#pragma once

#ifndef _NC_PARSER_IMPL_H_
#define _NC_PARSER_IMPL_H_

#include "data_format.h"
#include "data_desc.h"

#include "file_io.h"

namespace NCFileIO
{
	using namespace NCData;

	struct TagInfoException : public std::exception
	{
		virtual const char* what() const override
		{
			return "Tag info error.";
		}
	};

	struct TagSNException : public TagInfoException
	{
		TagSNException(uint es, uint as)
			: ExpectSN(es), ActualSN(as) {}
		virtual const char* what() const override
		{
			return "Serial number does not match.";
		}

		uint ExpectSN;
		uint ActualSN;
	};

	struct TagFieldTypeException : public TagInfoException
	{
		TagFieldTypeException(TagType tt, DataType dt)
			: TagT(tt), DataT(dt) {}
		virtual const char* what() const override
		{
			return "Field type does not match.";
		}

		TagType TagT;
		DataType DataT;
	};

	class InputStream;

	class ParserImpl
	{
	public:
		ParserImpl(InputStream& input) : is(input), curSerialNum(0){}
		bool Parse(Desc* p_root_desc);

	private:
		InputStream& is;

		// Consume a field at tar with field desc.
		bool TryConsumeField(NCData::FieldDesc* p_desc, Byte* tar);

		uint curSerialNum;
	};

	NCData::uint GetPackedUnitsNum(InputStream& input);

	// Allocate memery from a descriptor.
	static Byte* AllocateFromDesc(Desc* p_desc);
	inline static uint GetDescByteSize(Desc* p_desc);

	static void SetDefalutValue(Byte* src, FieldDesc* p_fdesc);

	void ParseAnUnit(InputStream& input, NCData::TagType type, 
		Byte* tar, uint byte_size);

	template<typename T>
	inline void ReadVariant(InputStream& input, Byte* tar)
	{
		//Little endian.
		T& v = *((T*)tar);
		memset(&v, 0, sizeof(T));
		Byte b;
		int offset = 0;
		while (input.Read(&b, 1))
		{
			v = T(((b & 0x7F) << (offset*7))) | v ;
			if (!(b & 0x80)) break;//DO not have next byte.
			offset++;
		}
	}

	inline void DecodeZigzag32(Byte* tar)
	{
		uint& v = *((uint*)tar);
		v = (v >> 1) ^ -static_cast<int>(v & 1);
	}

	inline void DecodeZigzag64(Byte* tar)
	{
		uint64& v = *((uint64*)tar);
		v = (v >> 1) ^ -static_cast<int64>(v & 1);
	}
}

#endif // !_NC_PARSER_IMPL_H_



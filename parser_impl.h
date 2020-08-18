#pragma once

#ifndef _NC_PARSER_IMPL_H_
#define _NC_PARSER_IMPL_H_

#include "data_format.h"
#include "data_desc.h"

#include "file_io.h"

namespace NCFileIO
{
	using namespace NCData;

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

	void ParseAnUnit(InputStream& input, NCData::TagType type, 
		Byte* tar, uint byte_size);

	template<typename T>
	inline void ReadVariant(InputStream& input, Byte* tar)
	{
		//Little endian.
		T& v = *((T*)tar);
		Byte b;
		int offset = 0;
		while (input.Read(&b, 1))
		{
			v = ((b & 0x7F) << (offset*7)) | v ;
			//std::cout << NC_BITSET (v) << std::endl;
			if (!(b & 0x80)) break;//DO not have next one.
			offset++;
		}
	}

	template<typename T>
	inline void ReadZigzag(InputStream& input, Byte* tar)
	{
		ReadVariant<T>(input, tar);
		T& v = *((T*)tar);
		v = (-(v & 1) ^ (v >> 1));
	}
}

#endif // !_NC_PARSER_IMPL_H_



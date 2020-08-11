#pragma once

#ifndef _NC_PARSER_IMPL_H_
#define _NC_PARSER_IMPL_H_

#include "data_format.h"

namespace NCFileIO
{
	class InputStream;

	class ParserImpl
	{
	public:
		ParserImpl(InputStream& input) : is(input) {}
		bool Parse();


	private:
		InputStream& is;
	};

	NCData::uint GetPackedUnitsNum(InputStream& input);

	void ParseAnUnit(InputStream& input, NCData::TagType type);

	inline void ParseFloat(InputStream& input) {};
	//void ParseDouble(FileIO::InputStream& input);
	//void ParseInt32(FileIO::InputStream& input);
	//void ParseInt64(FileIO::InputStream& input);
	inline NCData::uint ParseUInt32(InputStream& input) { return 0; };
	//void ParseUInt64(FileIO::InputStream& input);
	//void ParseSInt32(FileIO::InputStream& input);
	//void ParseSInt64(FileIO::InputStream& input);

	template<NCData::TagType>
	void ParseForType(InputStream& input) {};

	/*	enum class TagType
	{
		FLOAT, DOUBLE,
			INT32, INT64,
			UINT32, UINT64,
			SINT32, SINT64,

			UNDEFINED
	};*/

	//Find a real parser for a TagType.
	#define TypeParseForward(_Type, _ParserFunc)\
	template<>\
	inline void ParseForType<_Type>(NCFileIO::InputStream& input){\
		_ParserFunc(input);}

	TypeParseForward(NCData::TagType::FLOAT, ParseFloat);


}

#endif // !_NC_PARSER_IMPL_H_



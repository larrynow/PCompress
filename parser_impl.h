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

	inline float ParseFloat(InputStream& input) {};
	inline double ParseDouble(InputStream& input) {};
	inline int ParseInt32(InputStream& input) { return 0; };
	inline NCData::int64 ParseInt64(InputStream& input) { return 0; };
	inline NCData::uint ParseUInt32(InputStream& input) { return 0; };
	inline NCData::uint64 ParseUInt64(InputStream& input) { return 0; };
	inline char ParseChar(InputStream& input) { return 0; };
	inline bool ParseBool(InputStream& input) { return 0; };

	template<NCData::DataType>
	inline void ParseForType(InputStream& input) {};

	//Find a real parser for a CPP Type.
	#define TypeParseForward(_Type, _ParserFunc)\
	template<>\
	inline void ParseForType<_Type>(NCFileIO::InputStream& input){\
		_ParserFunc(input);}

	TypeParseForward(NCData::DataType::FLOAT, ParseFloat);
	TypeParseForward(NCData::DataType::DOUBLE, ParseDouble);
	TypeParseForward(NCData::DataType::INT32, ParseInt32);
	TypeParseForward(NCData::DataType::INT64, ParseInt64);
	TypeParseForward(NCData::DataType::UINT32, ParseUInt32);
	TypeParseForward(NCData::DataType::UINT64, ParseUInt64);
	TypeParseForward(NCData::DataType::CHAR, ParseChar);
	TypeParseForward(NCData::DataType::BOOL, ParseBool);


}

#endif // !_NC_PARSER_IMPL_H_



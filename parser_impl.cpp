#include "parser_impl.h"
#include "file_io.h"
#include "tag_logic.h"

using namespace NCData;


bool NCFileIO::ParserImpl::Parse()
{
	const int tagBit = 0;
	while (is.Read((Byte*)&tagBit, sizeof(tagBit)))
	{
		auto tagType = NCData::TagType::FIXED32;
		bool isPacked = IsPacked(tagBit);

		for (uint i = 0; i < (isPacked ? GetPackedUnitsNum(is) : 1); i++)
		{
			ParseAnUnit(is, tagType);
		}
	}

	return false;
}

uint NCFileIO::GetPackedUnitsNum(InputStream& input)
{
	return ParseUInt32(input);
}

void NCFileIO::ParseAnUnit(InputStream& input, NCData::TagType type)
{
	using namespace NCData;

	
}


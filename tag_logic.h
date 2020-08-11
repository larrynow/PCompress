#pragma once
#ifndef _NC_TAG_LOGIC_H_
#define _NC_TAG_LOGIC_H_

#include"data_format.h"

#define NC_TAG_BIT			0x0000
#define NC_TAG_BIT_64		0x0001
#define NC_TAG_BIT_FIXED	0x0002
#define NC_TAG_BIT_SIGNED	0x0004
#define NC_TAG_BIT_PACKED	0x0008
#define NC_TAG_BIT_UNDEFINE	0xFFFF

namespace NCData
{
	struct TagInfo
	{
		virtual char GetValueType() = 0;
	};

	// Set a TagBit's packed flag.
	inline void PackTagBit(int& tagBit, bool packed = true) {
		tagBit = packed ? tagBit | NC_TAG_BIT_PACKED : tagBit;
	}

	#define NC_CASE_RETURN(_Case, _Ret)\
		case _Case:\
			return _Ret;\
			break;

	//Parse a TagType from a TagBit.
	TagType GetTagInfo(int tagBit)
	{
		switch (tagBit)
		{
			NC_CASE_RETURN(NC_TAG_BIT, TagType::UINT32)
			NC_CASE_RETURN(NC_TAG_BIT_64, TagType::UINT64)
			NC_CASE_RETURN(NC_TAG_BIT_FIXED, TagType::FIXED32)
			NC_CASE_RETURN(NC_TAG_BIT_64 | NC_TAG_BIT_FIXED, TagType::FIXED64)
			NC_CASE_RETURN(NC_TAG_BIT_SIGNED, TagType::SINT32)
			NC_CASE_RETURN(NC_TAG_BIT_64 | NC_TAG_BIT_SIGNED, TagType::SINT64)
		default:
			return TagType::UNDEFINED;
		}
	}

	inline bool IsPacked(int tag) {
		return tag | NC_TAG_BIT_PACKED;
	}

}

#endif // !_NC_TAG_LOGIC_H_

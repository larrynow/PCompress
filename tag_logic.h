#pragma once
#ifndef _NC_TAG_LOGIC_H_
#define _NC_TAG_LOGIC_H_

#include"data_format.h"

/*
* TagBit : 8-bits(Byte) info for a field value.
*/

#define NC_TAG_BIT			0x00
#define NC_TAG_BIT_64		0x01
#define NC_TAG_BIT_FIXED	0x02
#define NC_TAG_BIT_SIGNED	0x04
#define NC_TAG_BIT_PACKED	0x08
#define NC_TAG_BIT_UNDEFINE	0x0F

namespace NCData
{
	struct TagInfo
	{
		TagInfo(int _n, TagType _t) :  type(_t), sn(_n){}
		TagType type;
		uint sn;
	};

	// Set a TagBit's packed flag.
	inline void PackTagBit(UByte& tagBit, bool packed = true) {
		tagBit = packed ? tagBit | NC_TAG_BIT_PACKED : tagBit;
	}

	#define NC_CASE_RETURN(_Case, _Ret)\
		case _Case:\
			return _Ret;\
			break;

	inline int GetTagSN(UByte tagBits)
	{
		return tagBits >> 4;
	}
	//Parse a TagType from a TagBit.
	inline TagType GetTagType(UByte tagBits)
	{
		switch (tagBits)
		{
			NC_CASE_RETURN(NC_TAG_BIT, TagType::VARIANT32)
			NC_CASE_RETURN(NC_TAG_BIT_64, TagType::VARIANT64)
			NC_CASE_RETURN(NC_TAG_BIT_FIXED, TagType::FIXED32)
			NC_CASE_RETURN(NC_TAG_BIT_64 | NC_TAG_BIT_FIXED, TagType::FIXED64)
			NC_CASE_RETURN(NC_TAG_BIT_SIGNED, TagType::SINT32)
			NC_CASE_RETURN(NC_TAG_BIT_FIXED | NC_TAG_BIT_SIGNED, TagType::F_SINT32)
			NC_CASE_RETURN(NC_TAG_BIT_64 | NC_TAG_BIT_SIGNED, TagType::SINT64)
			NC_CASE_RETURN(NC_TAG_BIT_FIXED | NC_TAG_BIT_64 | NC_TAG_BIT_SIGNED, TagType::F_SINT64)
			NC_CASE_RETURN(NC_TAG_BIT_UNDEFINE, TagType::UNDEFINED)
		default:
			return TagType::UNDEFINED;
		}
	}

	inline TagInfo GetTagInfo(UByte tagBits)
	{
		return { GetTagSN(tagBits), GetTagType(tagBits) };
	}

	inline UByte CalTagBits(TagType type)
	{
		switch (type)
		{
			NC_CASE_RETURN(TagType::VARIANT32, NC_TAG_BIT)
			NC_CASE_RETURN(TagType::VARIANT64, NC_TAG_BIT_64)
			NC_CASE_RETURN(TagType::FIXED32, NC_TAG_BIT_FIXED)
			NC_CASE_RETURN(TagType::FIXED64, NC_TAG_BIT_64 | NC_TAG_BIT_FIXED)
			NC_CASE_RETURN(TagType::SINT32, NC_TAG_BIT_SIGNED)
			NC_CASE_RETURN(TagType::F_SINT32, NC_TAG_BIT_FIXED | NC_TAG_BIT_SIGNED)
			NC_CASE_RETURN(TagType::SINT64, NC_TAG_BIT_64 | NC_TAG_BIT_SIGNED)
			NC_CASE_RETURN(TagType::F_SINT64, NC_TAG_BIT_FIXED | NC_TAG_BIT_64 | NC_TAG_BIT_SIGNED)
			NC_CASE_RETURN(TagType::UNDEFINED, NC_TAG_BIT_UNDEFINE)
		default:
			return  NC_TAG_BIT_UNDEFINE;
		}
	}

	inline bool IsPacked(int tag) {
		return tag | NC_TAG_BIT_PACKED;
	}

}

#endif // !_NC_TAG_LOGIC_H_

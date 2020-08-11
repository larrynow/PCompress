#pragma once
#ifndef _NC_TAG_LOGIC_H_
#define _NC_TAG_LOGIC_H_

#include"data_format.h"

//Tag Bit values.
#define NC_TAG_BIT			0x0000
#define NC_TAG_BIT_FLOAT	0x0001
#define NC_TAG_BIT_64		0x0002
#define NC_TAG_BIT_SIGNED	0x0004
#define NC_TAG_BIT_COMP		0x0008
#define NC_TAG_BIT_PACKED	0x0010
#define NC_TAG_BIT_UNDEFINE	0xFFFF

namespace NCData
{
	struct TagInfo
	{
		virtual char GetValueType() = 0;
	};

	/*struct TagIndicator
	{
		TagIndicator(int tagBit)
		{
			type = GetTagInfo(tagBit);
		};

		TagType type;
	};*/

	/*
	//Tag infos incicator.
	template<TagType>
	struct TagIndicator //: public TagInfo
	{
		TagType Tag;
		typedef char ValueType;
		static int GetTagBit() { return NC_TAG_BIT_UNDEFINE; }
	};

	#define NC_REG_TAG(_TagT, _ValueT, _Bit)\
	template<> struct TagIndicator<_TagT>{\
		TagType Tag;\
		typedef _ValueT ValueType;\
		static int GetTagBit() { return _Bit; }}

	NC_REG_TAG(TagType::FLOAT,	float,	NC_TAG_BIT_FLOAT);
	NC_REG_TAG(TagType::DOUBLE,	double,	NC_TAG_BIT_FLOAT | NC_TAG_BIT_64);
	NC_REG_TAG(TagType::INT32,	int,	NC_TAG_BIT);
	NC_REG_TAG(TagType::INT64,	int64,	NC_TAG_BIT_64);
	NC_REG_TAG(TagType::UINT32,	uint,	NC_TAG_BIT_COMP);
	NC_REG_TAG(TagType::UINT64,	uint64,	NC_TAG_BIT_COMP | NC_TAG_BIT_64);
	NC_REG_TAG(TagType::SINT32,	int,	NC_TAG_BIT_COMP | NC_TAG_BIT_SIGNED);
	NC_REG_TAG(TagType::SINT64,	int64,	NC_TAG_BIT_COMP | NC_TAG_BIT_SIGNED | NC_TAG_BIT_64);

	#define NC_GET_TAGBIT(_TagT) TagIndicator<_TagT>::GetTagBit()
	#define NC_GET_TAGVTYPE(_TagT) TagIndicator<_TagT>::ValueType
	*/

	// Set a TagBit's packed flag.
	inline void PackTagBit(int& tagBit, bool packed = true) {
		tagBit = packed ? tagBit | NC_TAG_BIT_COMP : tagBit;
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
			NC_CASE_RETURN(NC_TAG_BIT_FLOAT, TagType::FLOAT)
			NC_CASE_RETURN(NC_TAG_BIT_FLOAT | NC_TAG_BIT_64, TagType::DOUBLE)
			NC_CASE_RETURN(NC_TAG_BIT, TagType::FIXED32)
			NC_CASE_RETURN(NC_TAG_BIT_64, TagType::FIXED64)
			NC_CASE_RETURN(NC_TAG_BIT_COMP, TagType::UINT32)
			NC_CASE_RETURN(NC_TAG_BIT_COMP | NC_TAG_BIT_64, TagType::UINT64)
			NC_CASE_RETURN(NC_TAG_BIT_COMP | NC_TAG_BIT_SIGNED, TagType::SINT32)
			NC_CASE_RETURN(NC_TAG_BIT_COMP | NC_TAG_BIT_SIGNED | NC_TAG_BIT_64, TagType::SINT64)
		default:
			return TagType::UNDEFINED;
		}
	}

	inline bool IsPacked(int tag) {
		return tag | NC_TAG_BIT_PACKED;
	}

}

#endif // !_NC_TAG_LOGIC_H_

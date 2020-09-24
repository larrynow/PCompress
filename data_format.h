#pragma once

#ifndef _NC_DATA_FORMAT_H_
#define _NC_DATA_FORMAT_H_

#include<string>
#include<bitset>

#include <cstdarg>

#define NC_BITSET(_data) std::bitset<sizeof(decltype(_data)) * 8>(_data)

#include<vector>
#include<istream>

inline void SplitString(std::istream& is, const char diem, std::vector<std::string>& out)
{
	out.clear();
	std::string s;
	while (std::getline(is, s, diem))
		out.push_back(s);
}

namespace NCData
{
	using Byte = char;
	using UByte = unsigned char;

	using uint = unsigned int;
	using int64 = long long;
	using uint64 = unsigned long long;

	struct ByteArray
	{
		ByteArray() : size(0), use(0), bytes(nullptr) {}
		// Ctor with memory allocation.
		ByteArray(int i) : size(i), bytes(/*TAG MemoryPool*/new Byte[i]), use(0)
		{
			memset(bytes, 0, size);
		}
		// Ctor with allocated memory.
		ByteArray(int s, Byte* data) : bytes(data), size(s), use(s) {}
		~ByteArray() { delete[] bytes; };

		Byte* bytes;
		int size;
		int use;
	};

	// TODO tags need modify latter.
	enum class TagType
	{
		FIXED32,	FIXED64,
		SINT32,		SINT64,
		F_SINT32,	F_SINT64,
		VARIANT32,	VARIANT64,

		UNDEFINED
	};

	enum class DataType
	{
		FLOAT,	DOUBLE,
		INT32,	INT64,
		UINT32,	UINT64,
		BOOL,	CHAR,
		EMBEDDED,
		UNDEFINED
	};
	
	uint static GetByteSize(DataType type)
	{
		switch (type)
		{
		case NCData::DataType::FLOAT:
		case NCData::DataType::INT32:
		case NCData::DataType::UINT32:
			return 4;
		case NCData::DataType::DOUBLE:
		case NCData::DataType::INT64:
		case NCData::DataType::UINT64:
			return 8;
		case NCData::DataType::BOOL:
		case NCData::DataType::CHAR:
			return 1;
		case NCData::DataType::EMBEDDED:
			return 0;
		case NCData::DataType::UNDEFINED:
			return 0;
		default:
			return 0;
		}
	}
	
#define ATTR( attr ) attr
#define NC_CASE_1(_case) case _case
#define NC_CASE_2(_case, ...) NC_CASE_1(_case): ATTR(NC_CASE_1(__VA_ARGS__))
#define NC_CASE_3(_case, ...) NC_CASE_1(_case): ATTR(NC_CASE_2(__VA_ARGS__))
#define NC_CASE_4(_case, ...) NC_CASE_1(_case): ATTR(NC_CASE_3(__VA_ARGS__))
#define NC_CASE_5(_case, ...) NC_CASE_1(_case): ATTR(NC_CASE_4(__VA_ARGS__))
#define NC_CASE_6(_case, ...) NC_CASE_1(_case): ATTR(NC_CASE_5(__VA_ARGS__))

	// Check if tag type is not complied.
	inline bool CheckDataTag(TagType t_type, DataType d_type)
	{
		switch (t_type)
		{
		case NCData::TagType::FIXED32:
			return d_type == DataType::FLOAT || 
				d_type == DataType::INT32 || d_type == DataType::UINT32 ||
				d_type == DataType::BOOL || d_type == DataType::CHAR;
		case NCData::TagType::FIXED64:
			return d_type == DataType::DOUBLE || 
				d_type == DataType::INT64 || d_type == DataType::UINT64;
		case NCData::TagType::SINT32:
		case NCData::TagType::F_SINT32:
			return d_type == DataType::INT32;
		case NCData::TagType::SINT64:
		case NCData::TagType::F_SINT64:
			return d_type == DataType::INT64;
		case NCData::TagType::VARIANT32:
			return d_type == DataType::INT32 || d_type == DataType::UINT32;
		case NCData::TagType::VARIANT64:
			return d_type == DataType::INT64 || d_type == DataType::UINT64;
		case NCData::TagType::UNDEFINED:
		default:
			return false;
		}
	}


}

#endif // !_NC_DATA_FORMAT_H_

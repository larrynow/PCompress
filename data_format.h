#pragma once

#ifndef _NC_DATA_FORMAT_H_
#define _NC_DATA_FORMAT_H_

#include<string>
#include<bitset>

#define NC_BITSET(_data) std::bitset<sizeof(decltype(_data)) * 8>(_data)

namespace NCData
{
	using Byte = char;

	using uint = unsigned int;
	using int64 = long long;
	using uint64 = unsigned long long;

	struct ByteArray
	{
		ByteArray() : size(0), use(0), bytes(nullptr) {}
		// Ctor with memory allocation.
		ByteArray(int i) : size(i), bytes(/*TAG MemoryPool*/new Byte[i]), use(0) {}
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
		UINT32,		UINT64,

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
	
}

#endif // !_NC_DATA_FORMAT_H_

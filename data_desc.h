#pragma once

#ifndef _NC_DATA_DESC_H_
#define _NC_DATA_DESC_H_

#include "data_format.h"
#include<vector>

namespace NCData
{
	class Field
	{
		virtual void GetValueType() = 0;
	};

	class FieldInt32
	{
		using ValueT = int;
	};

	//Descriptor for a field.
	class FieldDesc
	{
	public:
		FieldDesc(DataType t, const std::string& n, uint o)
			: type(t), fieldName(n), offset(o) {}

		DataType Type() { return type; }
		std::string FieldName() { return fieldName; }
		uint Offset() { return offset; }

	private:

		DataType type;
		std::string fieldName;

		uint offset;
	};
	
	using Desc = std::vector<FieldDesc*>;

	static void LoadDesc(Desc& desc, const std::string& desc_file) {};

	static void CalculateOffset(Desc& desc);
}


#endif // !_NC_DATA_DESC_H_

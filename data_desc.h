#pragma once

#ifndef _NC_DATA_DESC_H_
#define _NC_DATA_DESC_H_

#include "data_format.h"
#include<vector>
#include<iostream>

namespace NCData
{
	//Descriptor for a field.
	class FieldDesc
	{
	public:
		FieldDesc(DataType t, const std::string& n, uint o)
			: type(t), fieldName(n), offset(o), unitCount(1) {}

		DataType Type() { return type; }
		std::string FieldName() { return fieldName; }
		uint Offset() { return offset; }
		uint UnitCount() { return unitCount; }
		uint UnitByteSize() { return NCData::GetByteSize(type); }
		uint TotalByteSize() { return unitCount * NCData::GetByteSize(type); }

	private:

		DataType type;
		std::string fieldName;

		uint offset;
		uint unitCount;

	};
	
	using Desc = std::vector<FieldDesc*>;

	inline std::ostream& operator<<(std::ostream& os, Desc& desc)
	{
		for (auto f_d : desc)
		{
			os << "Offset : " << f_d->Offset() << std::endl;
		}
		return os;
	}

	static void LoadDesc(Desc& desc, const std::string& desc_file) {};

	static void CalculateOffset(Desc& desc);
}


#endif // !_NC_DATA_DESC_H_

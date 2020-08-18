#include "parser_impl.h"
#include "file_io.h"
#include "tag_logic.h"

using namespace NCData;

bool NCFileIO::ParserImpl::Parse(Desc* p_descriptor)
{
	struct S
	{
		S(int _i, uint _u, float _f, bool _b, double _d, char _c) :
			i(_i), u(_u), f(_f), b(_b), d(_d), c(_c) {}
		S() : i(0), u(0), f(0), b(0), d(0), c(0) {};

		int i;
		uint u;
		float f;
		bool b;
		double d;
		char c;
	};

	Byte* retData = AllocateFromDesc(p_descriptor);

	for (auto cur_field : *p_descriptor)
	{
		if (TryConsumeField(cur_field, retData+cur_field->Offset()))
		{
			
		}
	}
	S* s = (S*)retData;
	/*std::cout << "Dsplay S." << std::endl;
	std::cout << s->i << std::endl;
	std::cout << s->u << std::endl;
	std::cout << s->f << std::endl;
	std::cout << s->b << std::endl;
	std::cout << s->d << std::endl;
	std::cout << s->c << std::endl;
	std::cout << "Dsplay S Finish." << std::endl;*/

	return false;
}

uint NCFileIO::GetPackedUnitsNum(InputStream& input)
{
	return 1;
}

Byte* NCFileIO::AllocateFromDesc(Desc* p_desc)
{
	//TODO:MemoryPool;
	return new Byte[GetDescByteSize(p_desc)];
}

uint NCFileIO::GetDescByteSize(Desc* p_desc)
{
	//TODO:memorypadding.
	uint cnt = 0;
	for (auto f_desc : *p_desc)
	{
		cnt+=f_desc->TotalByteSize();
	}
	return cnt;
}

bool NCFileIO::ParserImpl::TryConsumeField(NCData::FieldDesc* p_field, Byte* tar)
{
	const Byte tagBits = 0;
	if (!is.Read((Byte*)&tagBits, sizeof(tagBits))) return false;//IS error;

	auto tagInfo = GetTagInfo(tagBits);
	if (curSerialNum != tagInfo.sn) return false;//Wrong field serial number.
	if (!CheckDataTag(tagInfo.type, p_field->Type())) return false;//Unexpect field type.
	
	// Start Parsing.
	for (size_t i = 0; i < p_field->UnitCount(); i++)
	{
		ParseAnUnit(is, tagInfo.type,
			tar + (i*p_field->UnitByteSize()), p_field->UnitByteSize());
		switch (p_field->Type())
		{
		case NCData::DataType::FLOAT:
			std::cout << *(float*)tar << std::endl;
			break;
		case NCData::DataType::DOUBLE:
			std::cout << *(double*)tar << std::endl;
			break;
		case NCData::DataType::INT32:
			std::cout << *(int*)tar << std::endl;
			break;
		case NCData::DataType::INT64:
			break;
		case NCData::DataType::UINT32:
			std::cout << *(int*)tar << std::endl;
			break;
		case NCData::DataType::UINT64:
			break;
		case NCData::DataType::BOOL:
			std::cout << *(bool*)tar << std::endl;
			break;
		case NCData::DataType::CHAR:
			std::cout << *(char*)tar << std::endl;
			break;
		case NCData::DataType::EMBEDDED:
			break;
		case NCData::DataType::UNDEFINED:
			break;
		default:
			break;
		}

	}

	return false;
}

void NCFileIO::ParseAnUnit(InputStream& input, NCData::TagType type, 
	Byte* tar, uint byte_size)
{
	switch (type)
	{
	case NCData::TagType::FIXED32:
	case NCData::TagType::FIXED64:
		input.Read(tar, byte_size);
		return;//Fixed encode, do nothing.

	case NCData::TagType::SINT32:
		ReadZigzag<int>(input, tar);
		return;
	case NCData::TagType::SINT64:
		ReadZigzag<int64>(input, tar);
		return;

	case NCData::TagType::VARIANT32:
		ReadVariant<int>(input, tar);
		return;
	case NCData::TagType::VARIANT64:
		ReadVariant<int64>(input, tar);
		return;

	case NCData::TagType::UNDEFINED:
	default:
		break;
	}
	
	return;//Tag Error.
}


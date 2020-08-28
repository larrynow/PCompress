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
		try
		{
			TryConsumeField(cur_field, retData + cur_field->Offset());
		}
		catch (TagSNException & e)
		{
			//Skip this field and set it with a default value.
			//TODO : default value.
			std::cout << "Skip" << std::endl;
			continue;
		}
		catch (TagFieldTypeException & e)
		{
			std::cout <<TagInfoException(e).what() << e.what() << std::endl;
			std::cout << (int)e.TagT << "---"<< (int)e.DataT << std::endl;
		}
	}
	S* s = (S*)retData;
	std::cout << "Dsplay S." << std::endl;
	std::cout << s->i << std::endl;
	std::cout << s->u << std::endl;
	std::cout << s->f << std::endl;
	std::cout << s->b << std::endl;
	std::cout << s->d << std::endl;
	std::cout << s->c << std::endl;
	std::cout << "Dsplay S Finish." << std::endl;

	return false;
}

uint NCFileIO::GetPackedUnitsNum(InputStream& input)
{
	return 1;
}

Byte* NCFileIO::AllocateFromDesc(Desc* p_desc)
{
	//TODO:MemoryPool;
	uint byteSize = GetDescByteSize(p_desc);
	auto ret = new Byte[byteSize];
	return ret;
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

void NCFileIO::SetDefalutValue(Byte* src, FieldDesc* p_desc)
{
	switch (p_desc->Type())
	{
	case NCData::DataType::FLOAT:
	case NCData::DataType::INT32:
	case NCData::DataType::UINT32:
		memset(src, 0, sizeof(float));
		break;
	case NCData::DataType::DOUBLE:
	case NCData::DataType::INT64:
	case NCData::DataType::UINT64:
		memset(src, 0, sizeof(double));
		break;
	case NCData::DataType::BOOL:
	case NCData::DataType::CHAR:
		memset(src, 0, sizeof(char));
		break;
	case NCData::DataType::EMBEDDED:
	case NCData::DataType::UNDEFINED:
	default:
		break;
	}
}

bool NCFileIO::ParserImpl::TryConsumeField(NCData::FieldDesc* p_field, Byte* tar)
{
	Byte tagBits = 0;
	if (!is.Read((Byte*)&tagBits, sizeof(tagBits))) 
		throw FileIOException();//IS error;

	auto tagInfo = GetTagInfo(tagBits);
	if (tagInfo.type == TagType::UNDEFINED)//Field set with nothing, use default value.
	{
		SetDefalutValue(tar, p_field);
		return false;
	}
	if (curSerialNum != tagInfo.sn)
		throw TagSNException();//Serial number does not matach.
	if (!CheckDataTag(tagInfo.type, p_field->Type())) 
		throw TagFieldTypeException(tagInfo.type, p_field->Type());//Unexpect field type.
	
	// Start Parsing.
	for (size_t i = 0; i < p_field->UnitCount(); i++)
	{
		ParseAnUnit(is, tagInfo.type,
			tar + (i*p_field->UnitByteSize()), p_field->UnitByteSize());
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
		ReadVariant<int>(input, tar);
		DecodeZigzag<int>(tar);
		return;
	case NCData::TagType::F_SINT32:
		input.Read(tar, byte_size);
		DecodeZigzag<int>(tar);
		return;

	case NCData::TagType::SINT64:
		ReadVariant<int64>(input, tar);
		DecodeZigzag<int64>(tar);
		return;
	case NCData::TagType::F_SINT64:
		input.Read(tar, byte_size);
		DecodeZigzag<int64>(tar);
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


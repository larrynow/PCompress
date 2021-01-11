#include "parser_impl.h"
#include "file_io.h"
#include "tag_logic.h"
#include "utils.h"

using namespace NCData;

bool NCFileIO::ParserImpl::Parse(Desc* p_descriptor, Byte* target)
{
	/*struct S
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
	};*/
	if (!p_descriptor || p_descriptor->empty())
	{
		Log("Decriptor empty! Stop Parsing.");
		return false;
	}
	for (auto cur_field : *p_descriptor)
	{
		try
		{
			TryConsumeField(cur_field, target + cur_field->Offset());
		}
		catch (TagSNException & e)
		{
			std::cout << TagSNException(e).what() << e.what() << std::endl;
			std::cout << "Expect [" << e.ExpectSN << "] but got [" 
				<< e.ActualSN <<"]" << std::endl;
			return false;//Terminate when SN is not matched.
		}
		catch (TagFieldTypeException & e)
		{
			std::cout <<TagInfoException(e).what() << e.what() << std::endl;
			std::cout << (int)e.TagT << "---"<< (int)e.DataT << std::endl;
		}
	}
	/*S* s = (S*)retData;
	std::cout << "Dsplay S." << std::endl;
	std::cout << s->i << std::endl;
	std::cout << s->u << std::endl;
	std::cout << s->f << std::endl;
	std::cout << s->b << std::endl;
	std::cout << s->d << std::endl;
	std::cout << s->c << std::endl;
	std::cout << "Dsplay S Finish." << std::endl;*/

	return true;
}

uint NCFileIO::GetPackedUnitsNum(InputStream& input)
{
	return 1;
}

bool NCFileIO::ParserImpl::TryConsumeField(NCData::FieldDesc* p_field, Byte* tar)
{
	Byte tagBits = 0;
	if (!is.Read((Byte*)&tagBits, sizeof(tagBits)))//IS error;
		throw FileIOException();

	auto tagInfo = GetTagInfo(tagBits);
	if (tagInfo.type == TagType::UNDEFINED)//Field set with nothing, use default value.
	{
		SetDefalutValue(tar, p_field);
		return false;
	}
	if (curSerialNum != tagInfo.sn)//Serial number does not matach.
		throw TagSNException(curSerialNum, tagInfo.sn);
	if (!CheckDataTag(tagInfo.type, p_field->Type()))//Unexpect field type.
	{
		//Skip this field and set it with a default value.
		SetDefalutValue(tar, p_field);
		throw TagFieldTypeException(tagInfo.type, p_field->Type());
	}
	
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
	//Fixed encode, do nothing.
	case NCData::TagType::FIXED32:
	case NCData::TagType::FIXED64:
		input.Read(tar, byte_size);
		return;

	//Sint with zigzag.
	case NCData::TagType::SINT32:
		ReadVariant<int>(input, tar);
		DecodeZigzag32(tar);
		return;
	case NCData::TagType::F_SINT32:
		input.Read(tar, byte_size);
		DecodeZigzag32(tar);
		return;

	//Sint64 with zigzag.
	case NCData::TagType::SINT64:
		ReadVariant<int64>(input, tar);
		DecodeZigzag64(tar);
		return;
	case NCData::TagType::F_SINT64:
		input.Read(tar, byte_size);
		DecodeZigzag64(tar);
		return;
	
	//Variant encoded.
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


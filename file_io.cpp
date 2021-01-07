#include "file_io.h"

#include"parser_impl.h"
#include"encoder_impl.h"

#include"log.h"

using namespace NCData;

NCFileIO::Parser::Parser(const FilePath& _file) : input(_file)
{
	pimpl = new ParserImpl(input);
}

bool NCFileIO::Parser::Parse(Desc* p_desc, Byte* taregt)
{
	if (!pimpl->Parse(p_desc, taregt))
	{
		return false;
	}

	return true;
}

NCFileIO::Encoder::Encoder(const FilePath& _file) :output(_file)
{
	pimpl = new EncoderImpl(output);
}

bool NCFileIO::Encoder::Encode(Desc* p_descriptor, Byte* src_data)
{
	// Encode.
	try
	{
		pimpl->Encode(p_descriptor, src_data);
	}
	catch (EncodeException e)
	{
		NC_PR_LINE(e.what());
		return false;
	}

	return true;
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
		cnt += f_desc->TotalByteSize();
	}
	return cnt;
}

void NCFileIO::SetDefalutValue(Byte* src, FieldDesc* p_desc)
{
	//Set with field default value, all 0 bits filled.
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

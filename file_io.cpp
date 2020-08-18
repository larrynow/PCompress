#include "file_io.h"

#include"parser_impl.h"
#include"encoder_impl.h"

#include"log.h"

NCFileIO::Parser::Parser(const FilePath& _file) : input(_file)
{
	pimpl = new ParserImpl(input);
}

bool NCFileIO::Parser::Parse(Desc* p_desc)
{
	if (!pimpl->Parse(p_desc))
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

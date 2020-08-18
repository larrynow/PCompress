#ifndef _NC_FILE_IO_H_
#define _NC_FILE_IO_H_

#include<iostream>
#include<fstream>

#include"data_desc.h"

namespace NCFileIO
{
	using namespace NCData;

	using FilePath = std::string;

	struct FileIOException : public std::exception
	{
		virtual const char* what() const override
		{
			return "File IO Error.";
		}
	};

	struct BadFilePathException : public FileIOException
	{
		BadFilePathException(const std::string& _path) : path(_path) {}

		virtual const char*  what() const override
		{
			return ("File Path Error for [" + path + "].").c_str();
		}

	private:
		std::string path;
	};

	class InputStream
	{
	public:
		// From a file to inputStream;
		InputStream(const FilePath& file)
		{
			ifs.open(file.c_str(), std::ios::in | std::ios::binary);
			if (!ifs) throw BadFilePathException(file);
		}

		bool Read(Byte* target, int size)
		{
			if (ifs.read(target, size))
				return true;
			else
				return false;
		}

		~InputStream()
		{
			ifs.close();
		}

	private:
		std::ifstream ifs;
	};

	class OutputStream
	{
	public:
		// From a file to outputStream;
		OutputStream(const FilePath& file)
		{
			os.open(file.c_str(), std::ios::out | std::ios::binary);
			if (!os) throw BadFilePathException(file);
		}

		bool Write(Byte* src, int size)
		{
			if (os.write(src, size))
				return true;
			else
				return false;
		}

		~OutputStream()
		{
			os.close();
		}

	private:
		std::ofstream os;
	};

	class ParserImpl;
	class Parser
	{
	public:
		Parser(const FilePath& _file);
		
		// Main Parse Loop;
		bool Parse(Desc* p_desc);

	private:
		InputStream input;
		ParserImpl* pimpl;
	};

	class EncoderImpl;
	class Encoder
	{
	public:
		Encoder(const FilePath& _file);

		// Encode src_data to outputstream with format defined by descriptor.
		bool Encode(Desc* p_descriptor, Byte* src_data);

	private:
		OutputStream output;
		EncoderImpl* pimpl;
	};

}
#endif // !_NC_FILE_IO_H_
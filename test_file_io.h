#pragma once

#ifndef _NC_TEST_FILE_IO_H_
#define _NC_TEST_FILE_IO_H_

#include "test_base.h"
#include "file_io.h"

namespace NCTest
{
	class TestFileIO : public TestBase
	{
	public:
		virtual void Test()
		{
			using namespace NCFileIO;

			int i = 1;
			char v = 20;

			std::ofstream outFile("C:\\Users\\Administrator\\Desktop\\binary.txt", std::ios::out | std::ios::binary);
			outFile.write((char*)&i, sizeof(i));
			outFile.write((char*)&v, sizeof(v));
			outFile.close();

			try
			{
				InputStream is("C:\\Users\\Administrator\\Desktop\\binary.txt");
				int i;
				char val;
				is.Read((Byte*)&i, sizeof(int));
				std::cout << (int)i << std::endl;
				is.Read(&val, sizeof(char));
				std::cout << (int)val << std::endl;

			}
			catch (BadFilePathException e)
			{
				std::cout << e.what() << std::endl;
			}
		}
	};
}

#endif // !_NC_TEST_FILE_IO_H_

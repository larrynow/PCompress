#pragma once
#ifndef _NC_TEST_PARSER_H_
#define _NC_TEST_PARSER_H_

#include "test_base.h"
#include "file_io.h"
#include "parser_impl.h"

namespace NCTest
{
	struct S
	{
		S(int _i, uint _u, float _f, bool _b, double _d, char _c) :
			i(_i), u(_u), f(_f), b(_b), d(_d), c(_c) {}
		S() : i(), f(), b(), d(), c() {};

		friend std::ostream& operator<<(std::ostream& os, const S& s);

		int i;
		uint u;
		float f;
		bool b;
		double d;
		char c;
	};
	std::ostream& operator<<(std::ostream& os, const S& s)
	{
		os << s.i << std::endl;
		os << s.u << std::endl;
		os << s.f << std::endl;
		os << s.b << std::endl;
		os << s.d << std::endl;
		os << s.c << std::endl;
		return os;
	}

	using namespace NCData;

	class TestParser : public TestBase
	{
	public:
		virtual void Test()
		{
			using namespace NCFileIO;

			// Descriptor.
			Desc descs;
			descs.push_back(new FieldDesc(DataType::INT32, "intV", offsetof(S, i)));
			/*descs.push_back(new FieldDesc(DataType::UINT32, "intV", offsetof(S, u)));
			descs.push_back(new FieldDesc(DataType::FLOAT, "floatV", offsetof(S, f)));
			descs.push_back(new FieldDesc(DataType::BOOL, "bV", offsetof(S, b)));
			descs.push_back(new FieldDesc(DataType::DOUBLE, "dV", offsetof(S, d)));
			descs.push_back(new FieldDesc(DataType::CHAR, "cV", offsetof(S, c)));*/
			std::cout << descs << std::endl;

			S s = S(666, 777, 3.15f, true, 2.54433435, 't');

			// Encode.
			Encoder* encoder =
				new Encoder("C:\\Users\\Administrator\\Desktop\\binary.txt");
			encoder->Encode(&descs, (Byte*)&s);
			delete encoder;
			NC_PR_LINE("Encoding finish.");
			NC_PR_CROSS_LINE;

			// Parser test.
			try
			{
				/*Parser* parser =
					new Parser("C:\\Users\\Administrator\\Desktop\\binary.txt");
				parser->Parse(&descs);*/
				Byte* tar = new Byte[4];
				memset(tar, 0, 4 * sizeof(Byte));
				InputStream is(("C:\\Users\\Administrator\\Desktop\\binary.txt"));
				NCFileIO::ReadVariant<int>(is, tar);
				std::cout << *((int*)tar) << std::endl;
			}
			catch (BadFilePathException e)
			{
				std::cout << e.what() << std::endl;
			}
			NC_PR_LINE("Parser testing pass.");
		}
	};
}



#endif // !_NC_TEST_PARSER_H_


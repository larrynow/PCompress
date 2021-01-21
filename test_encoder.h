#pragma once
#ifndef _NC_TEST_ENCODER_H_
#define _NC_TEST_ENCODER_H_

#include "test_base.h"
#include "file_io.h"
#include "encoder_impl.h"

namespace NCTest 
{
	using namespace NCData;

	class TestEncoder : public TestBase
	{
	public:
		virtual void Test()
		{
			using namespace NCFileIO;

			// Descriptor.
			Desc descs;

			struct S
			{
				S(int _i, uint _u, float _f, bool _b, double _d, char _c) :
					i(_i), u(_u), f(_f), b(_b), d(_d), c(_c) {}
				S() : i(), u(), f(), b(), d(), c() {};

				int i;
				uint u;
				float f;
				bool b;
				double d;
				char c;
			};

			descs.push_back(new FieldDesc(DataType::INT32, "intV", offsetof(S, i)));
			descs.push_back(new FieldDesc(DataType::UINT32, "intV", offsetof(S, u)));
			descs.push_back(new FieldDesc(DataType::FLOAT, "floatV", offsetof(S, f)));
			descs.push_back(new FieldDesc(DataType::BOOL, "bV", offsetof(S, b)));
			descs.push_back(new FieldDesc(DataType::DOUBLE, "dV", offsetof(S, d)));
			descs.push_back(new FieldDesc(DataType::CHAR, "cV", offsetof(S, c)));

			for (auto d : descs)
			{
				std::cout << d->Offset() << std::endl;
			}

			auto testSample = [&descs](const S& s, int int_size)
			{
				NC_PR_LINE(NC_BITSET(s.i));
				Encoder* encoder = new Encoder("C:\\Users\\Administrator\\Desktop\\binary.txt");
				encoder->Encode(&descs, (Byte*)&s);
				delete encoder;
				NC_PR_LINE("Encoding finish.");
				NC_PR_CROSS_LINE;

				try
				{
					int i = 0;
					uint u = 0;
					float f = 0.0f;
					bool b = false;
					double d = 0.0;
					char c = ' ';

					InputStream is("C:\\Users\\Administrator\\Desktop\\binary.txt");
					// Little endian.
					is.Read((Byte*)(&i), int_size);
					is.Read((Byte*)(&i), 4);
					is.Read((Byte*)(&f), 4);
					is.Read((Byte*)(&b), 1);
					is.Read((Byte*)(&d), 8);
					is.Read((Byte*)(&c), 1);

					/*NC_PR_LINE(i);
					NC_PR_LINE(f);
					NC_PR_LINE(b);
					NC_PR_LINE(d);
					NC_PR_LINE(c);*/
				}
				catch (BadFilePathException e)
				{
					std::cout << e.what() << std::endl;
				}

				NC_PR_LINE("Encoder testing pass.");
			};

			testSample({ 666, 777, 3.14f, true, 2.23343499, 'x' }, 2);
			testSample({ INT_MAX, UINT_MAX, 3.14f, true, 2.23343499, 'x' }, 4);
			testSample({ -64, 0, 3.14f, true, 2.23343499, 'x' }, 4);
			testSample({ INT_MIN, 1, 3.14f, true, 2.23343499, 'x' }, 4);
		}
	};
}


#endif // !_NC_TEST_ENCODER_H_


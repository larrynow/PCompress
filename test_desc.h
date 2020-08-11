#pragma once
#ifndef _NC_TEST_DESC_H_
#define _NC_TEST_DESC_H_

#include "test_base.h"
#include "data_desc.h"

namespace NCTest
{
	using namespace NCData;

	class TestDesc : public TestBase
	{
	public:
		virtual void Test()
		{
			Desc descs;

			/*
			*TODO : Load a desc from file.
			*/
			//LoadDesc(descs, "");

			struct S
			{
				int i;
				float f;
				bool b;
				int64 i64;
				char c;
			};

			//NC_PR_CROSS_LINE;

			int offset = 0;
			descs.push_back(new FieldDesc(CPPType::INT32, "intV", offsetof(S, i)));
			descs.push_back(new FieldDesc(CPPType::FLOAT, "floatV", offsetof(S, f)));
			descs.push_back(new FieldDesc(CPPType::BOOL, "bV", offsetof(S, b)));
			descs.push_back(new FieldDesc(CPPType::INT64, "lV", offsetof(S, i64)));
			descs.push_back(new FieldDesc(CPPType::CHAR, "cV", offsetof(S, c)));

			for (auto d : descs)
			{
				std::cout << d->Offset() << std::endl;
			}

		}
	};
}

#endif // !_NC_TEST_DESC_H_H
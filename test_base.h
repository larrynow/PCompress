#pragma once
#ifndef _NC_TEST_BASE_H_
#define _NC_TEST_BASE_H_

#include"log.h"

namespace NCTest
{
	class TestBase
	{
		virtual void Test() = 0;
	};
}
#endif // !_NC_TEST_BASE_H_

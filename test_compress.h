#pragma once
#ifndef _NC_TEST_COMPRESS_H_
#define _NC_TEST_COMPRESS_H_

#include "test_base.h"
#include "neuron_compress.h"
namespace NCTest
{
	class TestCompress : public TestBase
	{
	public:
		
		virtual void Test() override
		{
			using namespace NCNeuron;

			std::string neuron_file("C://Projects//PCompress//neurons//Cell-16-2_1.CNG.swc");
			NeuronCompressor::Compress(neuron_file, 1);
		}

	};

}

#endif // !_NC_TEST_COMPRESS_H_

#pragma once
#ifndef _NC_TEST_COMPRESS_H_
#define _NC_TEST_COMPRESS_H_

#include "test_base.h"
#include "neuron_compress.h"
#include "neuron_logic.h"
#include "file_io.h"

namespace NCTest
{
	class TestCompress : public TestBase
	{
	public:
		
		virtual void Test() override
		{
			using namespace NCNeuron;

			int compress_level = 3;

			//std::string neuron_name = "AA0029.swc_rep";
			std::string neuron_name = "AA0249.swc_rep";
			//std::string neuron_name = "Cell-16-2_1.CNG";
			std::string neuron_file(".//neurons//"+neuron_name+".swc");
			NeuronCompressor::Compress(neuron_file, compress_level);

			// Decompress test.
			NeuronTree* nt = nullptr;
			try
			{
				nt = NeuronCompressor::Decompress(".//neurons//"
					+neuron_name+".ncc");
			}
			catch (NCFileIO::BadFilePathException& e)
			{
				std::cout << "File error!" << e.what()<< std::endl;
				return;
			}
			SaveSWC(*nt, ".//neurons//"+neuron_name+"_new3.swc");
			delete nt;
		}

	};

}

#endif // !_NC_TEST_COMPRESS_H_

#include"test_file_io.h"
#include"test_desc.h"
#include"test_encoder.h"
#include"test_parser.h"
#include"test_spline.h"
#include"test_compress.h"
#include"global_setting.h"
#include"utils.h"
#include"neuron_compress.h"

int main(int argc, char* argv[])
{
	using namespace NCTest;
	using namespace NCNeuron;

	GlobalSetting::SetDecimalPrecision(6);
	GlobalSetting::SetCurveThreshold(20.f);

	// File io test.
	/*auto fileioTest = new TestFileIO();
	fileioTest->Test();*/

	// Descriptor test.
	/*auto descTest = new TestDesc();
	descTest->Test();*/

	// Encoder test.
	/*auto encoderTest = new TestEncoder();
	encoderTest->Test();*/

	// Parser test.
	/*auto parseTest = new TestParser();
	parseTest->Test();*/

	// Compress test.
	/*auto compressTest = new TestCompress();
	compressTest->Test();*/

	// Spline test.
	/*auto spline_tset = TestSpline();
	spline_tset.Test();*/

	/*
	argv 1 : c/d/b.
		c: compress.
		d: decompress.
	argv 2 :
		if argv 1 is c: neuron file to compress.
		if argv 1 is d: decompressed file.
	argv 3 : 
		if argv 1 is c: compress level.
		if argv 1 is d: decompress target postfix(for input neuron file).
	*/

	char mode = 'c';

	if (argc > 4)
	{
		Log("Too many args! DO NOTHING!");
		return -1;
	}

	if (argc == 1 || argc == 2) return -1;// Too few args, do nothing.

	std::string file(argv[2]);
	mode = static_cast<char>(*argv[1]);
	switch (mode)
	{
		case 'c':
		{
			int level = 1;
			if(argc == 4)// Have level arg.
				level = static_cast<char>(*argv[3])-'0';

			Log("Compress : file[", file, "]. level : ", level);
			NeuronCompressor::Compress(file, level);
			break;
		}
		case 'd':
		{
			std::string postfix = "_new.swc";
			if (argc == 4)// Have level arg.
				postfix = std::string(argv[3]);

			auto nt = NeuronCompressor::Decompress(file);
			if (nt)
			{
				SaveSWC(*nt, file.substr(0, file.find_last_of('.')) + postfix);
				delete nt;
			}
			break;
		}
		default:
		{
			Log("Unsuporrted mode! Only [c,d] is supported.");
			break;
		}

	}

	return 0;
}
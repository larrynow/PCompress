#include "neuron_compress.h"
#include "file_io.h"
#include "neuron_representation.h"
#include "neuron_logic.h"

void NCNeuron::NeuronCompressor::Compress(const std::string& neuron_file, int level)
{
	using namespace NCFileIO;

	if (!CheckFile(neuron_file))
	{
		throw BadFilePathException(neuron_file);
	}
	auto neuron = ReadSWC_File(neuron_file);

	switch (level)
	{
		case 1:
		default:
		{
			auto ntc_file = neuron_file.substr(0, neuron_file.find_last_of('.')) + ".ntc";
			Level1Compress(neuron, ntc_file);
			break;
		}
		case 2:
		{
			auto ntc_file = neuron_file.substr(0, neuron_file.find_last_of('.')) + ".ntc";
			Level2Compress(neuron, ntc_file);
			break;
		}
		case 3:
		{
			auto nbc_file = neuron_file.substr(0, neuron_file.find_last_of('.')) + ".nbc";
			Level3Compress(neuron, nbc_file);
			break;
		}
	}

	std::cout << "Compress finish." << std::endl;
}

void NCNeuron::NeuronCompressor::Level1Compress(const NeuronTree& neuron, const std::string& out_file)
{
	using namespace NCFileIO;

	ClearFile(out_file);
	Encoder* encoder = new Encoder(out_file);

	// Encode neuron size;
	auto siz = neuron.size();
	auto siz_desc = new Desc();
	siz_desc->push_back(new FieldDesc(DataType::INT32, "size", 0));
	encoder->Encode(siz_desc, (Byte*)&siz);
	delete siz_desc;

	auto& desc = NeuronDescriptor::GetSWCDescriptor();

	int count = 0;
	for (auto& node : neuron)
	{
		std::cout << "Encode " << count++ << std::endl;
		encoder->Encode(&desc, (Byte*)&node);
	}

	delete encoder;

	std::cout << "Encode finish." << std::endl;
}

void NCNeuron::NeuronCompressor::Level2Compress(const NeuronTree& neuron, const std::string& out_file)
{

}

void NCNeuron::NeuronCompressor::Level3Compress(const NeuronTree& neuron, const std::string& out_file)
{

}

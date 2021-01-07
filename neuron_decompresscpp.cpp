#include"neuron_compress.h"
#include"file_io.h"
#include"neuron_logic.h"

NCNeuron::NeuronTree* NCNeuron::NeuronCompressor::Decompress(
	const std::string & com_file)
{
	using namespace NCFileIO;

	if (!CheckFile(com_file))
	{
		throw BadFilePathException(com_file);
	}

	auto suffix = com_file.substr(com_file.find_last_of('.'));

	if (suffix == ".ntc")
	{
		if (auto ret_data = Level1Decompress(com_file))
		{
			std::cout << "Level 1 decompress from"<< com_file << std::endl;
			return ret_data;
		}
	}
	else if (suffix == ".nsc")
	{
		if (auto ret_data = Level2Decompress(com_file))
		{
			std::cout << "Level 2 decompress from" << com_file << std::endl;
			return ret_data;
		}

	}
	else if (suffix == ".nlc")
	{
		if (auto ret_data = Level3Decompress(com_file))
		{
			std::cout << "Level 3 decompress from" << com_file << std::endl;
			return ret_data;
		}

	}
	else
	{
		std::cout << "Unsupported file type, do nothing." << std::endl;
	}

	return nullptr;
}

NCNeuron::NeuronTree* NCNeuron::NeuronCompressor::Level1Decompress(
	const std::string & com_file)
{
	using namespace NCFileIO;

	Parser* parser = new Parser(com_file);

	// Parse neuron size;
	int* siz = new int;
	auto siz_desc = new Desc();
	siz_desc->push_back(new FieldDesc(DataType::INT32, "size", 0));
	try
	{
		parser->Parse(siz_desc, (Byte*)siz);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << e.what() << std::endl;
	}
	delete siz_desc;

	NeuronTree* ret_neuron_tree = new NeuronTree(*siz);

	// Parse neuron tree;
	auto& desc = NeuronDescriptor::GetSWCDescriptor();
	for (int i = 0; i < *siz; i++)
	{
		try
		{
			parser->Parse(&desc, (Byte*)&(ret_neuron_tree->at(i)));
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << e.what() << std::endl;
		}
	}
	
	delete parser;
	delete siz;

	return ret_neuron_tree;
}

NCNeuron::NeuronTree* NCNeuron::NeuronCompressor::Level2Decompress(
	const std::string & com_file)
{
	using namespace NCFileIO;

	Parser* parser = new Parser(com_file);

	// Parse neuron size;
	int* siz = new int;
	auto siz_desc = new Desc();
	siz_desc->push_back(new FieldDesc(DataType::INT32, "size", 0));
	try
	{
		parser->Parse(siz_desc, (Byte*)siz);
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << e.what() << std::endl;
	}
	delete siz_desc;

	NeuronSplineTree* sp_tree = new NeuronSplineTree(*siz);
	// Parse neuron tree;
	auto& desc = NeuronDescriptor::GetSplineDescriptor();
	for (int i = 0; i < *siz; i++)
	{
		try
		{
			parser->Parse(&desc, (Byte*) & (sp_tree->at(i)));
		}
		catch (std::exception & e)
		{
			std::cout << e.what() << e.what() << std::endl;
		}
	}
	delete parser;
	delete siz;

	return new NeuronTree(GetNeuronTree(*sp_tree));
}

NCNeuron::NeuronTree* NCNeuron::NeuronCompressor::Level3Decompress(
	const std::string & com_file)
{
	return nullptr;
}
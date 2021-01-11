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
			std::cout << "Level 1 decompress from "<< com_file << std::endl;
			return ret_data;
		}
	}
	else if (suffix == ".nsc")
	{
		if (auto ret_data = Level2Decompress(com_file))
		{
			std::cout << "Level 2 decompress from " << com_file << std::endl;
			return ret_data;
		}

	}
	else if (suffix == ".ncc")
	{
		if (auto ret_data = Level3Decompress(com_file))
		{
			std::cout << "Level 3 decompress from " << com_file << std::endl;
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

	auto ret_tree = new NeuronTree(GetNeuronTree(*sp_tree));
	delete sp_tree;

	return ret_tree;
}

NCNeuron::NeuronTree* NCNeuron::NeuronCompressor::Level3Decompress(const std::string& com_file)
{
	using namespace NCFileIO;

	Parser* parser = new Parser(com_file);

	// Parse soma;
	VEC3 soma;
	auto som_desc = new Desc();
	som_desc->push_back({ new FieldDesc(DataType::FLOAT, "soma_x", offsetof(VEC3, x)) });
	som_desc->push_back({ new FieldDesc(DataType::FLOAT, "soma_y", offsetof(VEC3, y)) });
	som_desc->push_back({ new FieldDesc(DataType::FLOAT, "soma_z", offsetof(VEC3, z)) });
	try
	{
		parser->Parse(som_desc, (Byte*)&soma);
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << e.what() << std::endl;
		delete som_desc;
		return nullptr;
	}
	delete som_desc;

	// Parse group num;
	int group_num = 0;
	auto siz_desc = new Desc();
	siz_desc->push_back(new FieldDesc(DataType::INT32, "group_num", 0));
	try
	{
		parser->Parse(siz_desc, (Byte*)&group_num);
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << e.what() << std::endl;
		delete siz_desc;
	}

	CompactNeuronTree c_tree(group_num);
	c_tree.soma_point = soma;

	// Parse every subtree;
	for (auto& sub_tree : c_tree.sub_trees)
	{
		parser->Parse(siz_desc, (Byte*)&sub_tree.type);
		
		int group_size = 0;
		parser->Parse(siz_desc, (Byte*)&group_size);
		sub_tree.nodes.resize(group_size);

		auto& desc = NeuronDescriptor::GetCompactSplineNodeDescriptor();
		for (int i = 0; i < group_size; i++)
		{
			try
			{
				parser->Parse(&desc, (Byte*) & (sub_tree.nodes.at(i)));
			}
			catch (std::exception & e)
			{
				std::cout << e.what() << e.what() << std::endl;
			}
		}
	}
	delete siz_desc;
	delete parser;

	auto ret_tree = new NeuronTree(GetNeuronTree(c_tree));

	return ret_tree;
}

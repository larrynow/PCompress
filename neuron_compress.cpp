#include "neuron_compress.h"
#include "file_io.h"
#include "neuron_representation.h"
#include "neuron_logic.h"
#include "utils.h"

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
			auto nsc_file = neuron_file.substr(0, neuron_file.find_last_of('.')) + ".nsc";
			Level2Compress(neuron, nsc_file);
			break;
		}
		case 3:
		{
			auto nsc_file = neuron_file.substr(0, neuron_file.find_last_of('.')) + ".ncc";
			Level3Compress(neuron, nsc_file);
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
	for (auto& node : neuron)
	{
		encoder->Encode(&desc, (Byte*)&node);
	}
	delete encoder;

	std::cout << "Encode finish." << std::endl;
}

void NCNeuron::NeuronCompressor::Level2Compress(const NeuronTree& neuron, const std::string& out_file)
{
	using namespace NCFileIO;

	ClearFile(out_file);
	Encoder* encoder = new Encoder(out_file);

	// Get Spline ctree.
	auto b_tree = GetBranchTree<NCNeuron::NodeParam::SPLINE>(neuron);
	FitBranchTree(b_tree);
	auto tree = GetSplineTree(b_tree);
	delete b_tree;

	////Code to test int tangent-representation.
	//auto trans = [](float tan_v)
	//{
	//	float angle = CompactSplineNode::ReserveAngle(GetAngle(atan(tan_v)));
	//	auto ret = tan(GetRadian(angle));
	//	return ret;
	//};
	//for (auto& node : ctree)
	//{
	//	node.params.XParams = VEC4(node.params.XParams.x, node.params.XParams.y,
	//		trans(node.params.XParams.z),
	//		trans(node.params.XParams.w)
	//	);
	//	node.params.YParams = VEC4(node.params.YParams.x, node.params.YParams.y,
	//		trans(node.params.YParams.z),
	//		trans(node.params.YParams.w)
	//	);
	//	node.params.ZParams = VEC4(node.params.ZParams.x, node.params.ZParams.y,
	//		trans(node.params.ZParams.z),
	//		trans(node.params.ZParams.w)
	//	);
	//}

	// Encode ctree size;
	auto siz = tree.size();
	auto siz_desc = new Desc();
	siz_desc->push_back(new FieldDesc(DataType::INT32, "size", 0));
	encoder->Encode(siz_desc, (Byte*)&siz);
	delete siz_desc;

	auto& desc = NeuronDescriptor::GetSplineDescriptor();
	for (auto& node : tree)
	{
		encoder->Encode(&desc, (Byte*)&node);
	}
	delete encoder;

	std::cout << "Encode finish." << std::endl;

}

void NCNeuron::NeuronCompressor::Level3Compress(const NeuronTree& neuron, const std::string& out_file)
{
	using namespace NCFileIO;

	ClearFile(out_file);
	Encoder* encoder = new Encoder(out_file);

	// Get Spline ctree.
	auto b_tree = GetBranchTree<NCNeuron::NodeParam::SPLINE>(neuron);
	FitBranchTree(b_tree);
	auto ctree = CompactBranchSplineTree(b_tree);
	delete b_tree;

	auto& desc = NeuronDescriptor::GetCompactSplineNodeDescriptor();

	// First Save Soma.
	auto soma = ctree.soma_point;
	auto som_desc = new Desc();
	som_desc->push_back({ new FieldDesc(DataType::FLOAT, "soma_x", offsetof(VEC3, x)) });
	som_desc->push_back({ new FieldDesc(DataType::FLOAT, "soma_y", offsetof(VEC3, y)) });
	som_desc->push_back({ new FieldDesc(DataType::FLOAT, "soma_z", offsetof(VEC3, z)) });
	encoder->Encode(som_desc, (Byte*)&soma);

	// Save group num.
	auto int_desc = new Desc();
	int_desc->push_back(new FieldDesc(DataType::INT32, "int", 0));
	int group_num = ctree.sub_trees.size();
	encoder->Encode(int_desc, (Byte*)&group_num);

	// Then save every group(one type) nodes.
	for (auto& group : ctree.sub_trees)
	{
		auto group_type = group.type;
		encoder->Encode(int_desc, (Byte*)&group_type);
		auto group_size = group.nodes.size();
		encoder->Encode(int_desc, (Byte*)&group_size);

		for (auto& node : group.nodes)
		{
			encoder->Encode(
				&NeuronDescriptor::GetCompactSplineNodeDescriptor(), (Byte*)&node);
		}
	}
	
	delete encoder;

	std::cout << "Encode finish." << std::endl;
}

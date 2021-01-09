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
			auto ncc_file = neuron_file.substr(0, neuron_file.find_last_of('.')) + ".ncc";
			Level3Compress(neuron, ncc_file);
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

	// Get Spline tree.
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
	//for (auto& node : tree)
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

	// Encode tree size;
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
	/*
	Smiliar with level-2. But Compress Params a little.
	*/
	using namespace NCFileIO;

	ClearFile(out_file);
	Encoder* encoder = new Encoder(out_file);

	// Get Spline tree.
	auto b_tree = GetBranchTree<NCNeuron::NodeParam::COMPACTSPLINE>(neuron);
	CompactBranchTree(b_tree);
	auto tree = GetCompactSplineTree(b_tree);
	delete b_tree;

	// Encode tree size;
	auto siz = tree.size();
	auto siz_desc = new Desc();
	siz_desc->push_back(new FieldDesc(DataType::INT32, "size", 0));
	encoder->Encode(siz_desc, (Byte*)&siz);

	auto& spline_desc = NeuronDescriptor::GetSplineDescriptor();
	auto& param_desc = NeuronDescriptor::GetCompactParamDescriptor();
	for (auto& node : tree)
	{
		encoder->Encode(&spline_desc, (Byte*)&node);// Spline node part.
		encoder->Encode(siz_desc, (Byte*)&node.param_size);// Param sequence size.
		if (node.param_size != 0)
		{
			for (int i = 0; i < node.param_size; i++)
			{// Every Param.
				encoder->Encode(&param_desc, (Byte*)&(node.param_sequences[i]));
			}
		}
	}
	delete siz_desc;
	delete encoder;

	std::cout << "Encode finish." << std::endl;
}

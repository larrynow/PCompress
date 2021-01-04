#pragma once
#ifndef __NC_NEURON_REPRESENTATION_H__
#define __NC_NEURON_REPRESENTATION_H__

#include<vector>
#include<map>
#include"data_desc.h"

namespace NCNeuron
{
	using namespace NCData;

	struct NeuronSWCNode
	{
		int n;
		int type;
		float x, y, z;
		float radius;
		int parent;
	};

	using NeuronTree = std::vector<NeuronSWCNode>;

	const static int BRANCH_LATENT_SIZ = 5;
	struct NeuronBranchNode
	{
		int id;
		int p_id;

		float branch_latent[BRANCH_LATENT_SIZ];

		int direction[3];
		int v_direction[3];
	};

	using NeuronBranchTree = std::vector<NeuronBranchNode>;

	class NeuronDescriptor
	{
	public:
		~NeuronDescriptor()
		{
		}
		NeuronDescriptor(const NeuronDescriptor&) = delete;
		NeuronDescriptor& operator=(const NeuronDescriptor&) = delete;
		
		static Desc& GetSWCDescriptor()
		{
			static NeuronDescriptor instance;
			return instance.neuron_SWC_desc;
		}

		static Desc& GetBranchDescriptor()
		{
			static NeuronDescriptor instance;
			return instance.neuron_branch_desc;
		}

	private:
		NeuronDescriptor()
		{
			neuron_SWC_desc.clear();
			neuron_SWC_desc.push_back(new FieldDesc(DataType::INT32, "n", offsetof(NeuronSWCNode, n)));
			neuron_SWC_desc.push_back(new FieldDesc(DataType::INT32, "type", offsetof(NeuronSWCNode, type)));
			neuron_SWC_desc.push_back(new FieldDesc(DataType::FLOAT, "x", offsetof(NeuronSWCNode, x)));
			neuron_SWC_desc.push_back(new FieldDesc(DataType::FLOAT, "y", offsetof(NeuronSWCNode, y)));
			neuron_SWC_desc.push_back(new FieldDesc(DataType::FLOAT, "z", offsetof(NeuronSWCNode, z)));
			neuron_SWC_desc.push_back(new FieldDesc(DataType::FLOAT, "radius", offsetof(NeuronSWCNode, radius)));
			neuron_SWC_desc.push_back(new FieldDesc(DataType::INT32, "parent", offsetof(NeuronSWCNode, parent)));

			neuron_branch_desc.clear();
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "id", offsetof(NeuronBranchNode, id)));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "p_id", offsetof(NeuronBranchNode, p_id)));
			for (int i = 0; i < BRANCH_LATENT_SIZ; i++)
				neuron_branch_desc.push_back(new FieldDesc(DataType::FLOAT, std::string("latent_")+std::to_string(i), offsetof(NeuronBranchNode, branch_latent[i])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "direction_x", offsetof(NeuronBranchNode, direction[0])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "direction_y", offsetof(NeuronBranchNode, direction[1])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "direction_z", offsetof(NeuronBranchNode, direction[2])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "v_direction_x", offsetof(NeuronBranchNode, v_direction[0])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "v_direction_y", offsetof(NeuronBranchNode, v_direction[1])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "v_direction_z", offsetof(NeuronBranchNode, v_direction[2])));
		}
	
		Desc neuron_SWC_desc;
		Desc neuron_branch_desc;
	};

}

#endif // !__NC_NEURON_REPRESENTATION_H__

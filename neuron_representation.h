#pragma once
#ifndef __NC_NEURON_REPRESENTATION_H__
#define __NC_NEURON_REPRESENTATION_H__

#include<vector>
#include<map>
#include"data_desc.h"
#include"branch_tree.h"

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

	// A vector for swc nodes.
	using NeuronTree = std::vector<NeuronSWCNode>;

	// Latent node in vector.
	struct NeuronLatentNode
	{
		int id;
		int p_id;

		float branch_latent[BRANCH_LATENT_SIZ];

		int direction[3];
		int v_direction[3];
	};

	// A vector for latent nodes.
	using NeuronLatentTree = std::vector<NeuronLatentNode>;

	// Spline node in vector.
	struct NeuronSplineNode
	{
		int id;
		int p_id;
		int type;
		float radius;

		NCSplineCurve::SCParams params;
	};

	// A vector for spline nodes.
	using NeuronSplineTree = std::vector<NeuronSplineNode>;

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

		static Desc& GetSplineDescriptor()
		{
			static NeuronDescriptor instance;
			return instance.neuron_spline_desc;
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

			/*
			struct NeuronSplineNode
			{
				int id;
				int p_id;
				int type;
				float radius;

				NCSplineCurve::SCParams params;
			};
			*/
			neuron_spline_desc.clear();
			neuron_spline_desc.push_back(new FieldDesc(DataType::INT32, "id", offsetof(NeuronSplineNode, id)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::INT32, "p_id", offsetof(NeuronSplineNode, p_id)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::INT32, "type", offsetof(NeuronSplineNode, type)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "radius", offsetof(NeuronSplineNode, radius)));
			
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramX.x", offsetof(NeuronSplineNode, params.XParams.x)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramX.y", offsetof(NeuronSplineNode, params.XParams.y)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramX.z", offsetof(NeuronSplineNode, params.XParams.z)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramX.w", offsetof(NeuronSplineNode, params.XParams.w)));
			
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramY.x", offsetof(NeuronSplineNode, params.YParams.x)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramY.y", offsetof(NeuronSplineNode, params.YParams.y)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramY.z", offsetof(NeuronSplineNode, params.YParams.z)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramY.w", offsetof(NeuronSplineNode, params.YParams.w)));

			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramZ.x", offsetof(NeuronSplineNode, params.ZParams.x)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramZ.y", offsetof(NeuronSplineNode, params.ZParams.y)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramZ.z", offsetof(NeuronSplineNode, params.ZParams.z)));
			neuron_spline_desc.push_back(new FieldDesc(DataType::FLOAT, "paramZ.w", offsetof(NeuronSplineNode, params.ZParams.w)));

			neuron_branch_desc.clear();
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "id", offsetof(NeuronLatentNode, id)));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "p_id", offsetof(NeuronLatentNode, p_id)));
			for (int i = 0; i < BRANCH_LATENT_SIZ; i++)
				neuron_branch_desc.push_back(new FieldDesc(DataType::FLOAT, std::string("latent_")+std::to_string(i), offsetof(NeuronLatentNode, branch_latent[i])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "direction_x", offsetof(NeuronLatentNode, direction[0])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "direction_y", offsetof(NeuronLatentNode, direction[1])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "direction_z", offsetof(NeuronLatentNode, direction[2])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "v_direction_x", offsetof(NeuronLatentNode, v_direction[0])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "v_direction_y", offsetof(NeuronLatentNode, v_direction[1])));
			neuron_branch_desc.push_back(new FieldDesc(DataType::INT32, "v_direction_z", offsetof(NeuronLatentNode, v_direction[2])));
		
		}
	
		Desc neuron_SWC_desc;
		Desc neuron_branch_desc;
		Desc neuron_spline_desc;
	};

}

#endif // !__NC_NEURON_REPRESENTATION_H__

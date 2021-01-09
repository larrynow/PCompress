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

	// Spline node in vector.
	struct NeuronSplineNode
	{
		NeuronSplineNode() = default;

		NeuronSplineNode(int i, int p, int t, float r, NCSplineCurve::SCParams pa) : 
			id(i), p_id(p), type(t), radius(r), params(pa)
		{}
		int id;
		int p_id;
		int type;
		float radius;

		NCSplineCurve::SCParams params;
	};

	// A vector for spline nodes.
	using NeuronSplineTree = std::vector<NeuronSplineNode>;

	// A compact expression of spline params.
	struct CompactSplineNode : public NeuronSplineNode
	{
		CompactSplineNode() : NeuronSplineNode(),
			param_size(0), param_sequences(nullptr) {}

		CompactSplineNode(int id, int p_id, int type, float radius,
			NCSplineCurve::SCParams params,
			int p_size, BranchCompactSplineNode::CompactParam* p_sequences) : 
			NeuronSplineNode(id, p_id, type, radius, params), 
			param_size(p_size), param_sequences(nullptr)
		{
			if (param_size != 0)
			{
				param_sequences =
					new BranchCompactSplineNode::CompactParam[param_size];
				memcpy(param_sequences, p_sequences, 
					param_size*sizeof(BranchCompactSplineNode::CompactParam));
			}
		};

		CompactSplineNode(const CompactSplineNode& node) :
			CompactSplineNode(node.id, node.p_id, node.type, node.radius,
				node.params, node.param_size, node.param_sequences) {}

		~CompactSplineNode()
		{
			if (param_sequences) {
				delete[] param_sequences;
				param_sequences = nullptr;
			}
		}

		int param_size;
		BranchCompactSplineNode::CompactParam* param_sequences;
	};

	using NeuronCompactSplineTree = std::vector<CompactSplineNode>;

	class NeuronDescriptor
	{
	public:
		~NeuronDescriptor()
		{
		}
		NeuronDescriptor(const NeuronDescriptor&) = delete;
		NeuronDescriptor& operator=(const NeuronDescriptor&) = delete;
		
		static NeuronDescriptor& GetInstance()
		{
			static NeuronDescriptor instance;
			return instance;
		}

		static Desc& GetSWCDescriptor()
		{
			return GetInstance().neuron_SWC_desc;
		}

		static Desc& GetSplineDescriptor()
		{
			return GetInstance().neuron_spline_desc;
		}

		static Desc& GetCompactParamDescriptor()
		{
			return GetInstance().compact_param_desc;
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

			/*
			struct CompactParam
			{
				VEC3 position;

				int angle_int_x;// A angle to get a tangent.
				int angle_int_y;// A angle to get a tangent.
				int angle_int_z;// A angle to get a tangent.
			};
			*/
			compact_param_desc.clear();
			compact_param_desc.push_back(new FieldDesc(DataType::FLOAT, "position.x", 
				offsetof(BranchCompactSplineNode::CompactParam, position.x)));
			compact_param_desc.push_back(new FieldDesc(DataType::FLOAT, "position.y",
				offsetof(BranchCompactSplineNode::CompactParam, position.y)));
			compact_param_desc.push_back(new FieldDesc(DataType::FLOAT, "position.z",
				offsetof(BranchCompactSplineNode::CompactParam, position.z)));
			
			compact_param_desc.push_back(new FieldDesc(DataType::FLOAT, "angle_x", 
				offsetof(BranchCompactSplineNode::CompactParam, angle_int_x)));
			compact_param_desc.push_back(new FieldDesc(DataType::FLOAT, "angle_y",
				offsetof(BranchCompactSplineNode::CompactParam, angle_int_y)));
			compact_param_desc.push_back(new FieldDesc(DataType::FLOAT, "angle_z",
				offsetof(BranchCompactSplineNode::CompactParam, angle_int_z)));
		}
	
		Desc neuron_SWC_desc;
		Desc neuron_spline_desc;
		Desc compact_param_desc;
	};

}

#endif // !__NC_NEURON_REPRESENTATION_H__

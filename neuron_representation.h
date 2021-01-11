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

	// A compact expresion of neuron tree.
	struct CompactNeuronTree
	{
		CompactNeuronTree(int s) : sub_trees(s) {}

		VEC3 soma_point;

		struct CompactParam
		{
			VEC3 end_point;
			int start_tangent_angle_x;
			int start_tangent_angle_y;
			int start_tangent_angle_z;
			int end_tangent_angle_x;
			int end_tangent_angle_y;
			int end_tangent_angle_z;
		};

		struct CompactSplineNode
		{
			int id;
			int pid;
			float radius;

			CompactParam params;
		};

		struct Subtree
		{
			int type;
			std::vector<CompactSplineNode> nodes;
		};

		std::vector<Subtree> sub_trees;

		static CompactParam CompactAParam(
			const NCSplineCurve::SCParams& p)
		{
			return CompactParam({
				p.GetEndPoint(),
				GetAngle(atan(p.GetStartTangent().x)), 
				GetAngle(atan(p.GetStartTangent().y)),
				GetAngle(atan(p.GetStartTangent().z)),
				GetAngle(atan(p.GetEndTangent().x)),
				GetAngle(atan(p.GetEndTangent().y)),
				GetAngle(atan(p.GetEndTangent().z)),
				});
		};
	};

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

		}
	
		Desc neuron_SWC_desc;
		Desc neuron_spline_desc;
	};

}

#endif // !__NC_NEURON_REPRESENTATION_H__

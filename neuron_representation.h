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
		int id;
		int p_id;
		int type;
		float radius;

		NCSplineCurve::SCParams params;
	};

	// A vector for spline nodes.
	using NeuronSplineTree = std::vector<NeuronSplineNode>;

	// A compact expression of spline params.
	struct CompactSplineNode
	{
		CompactSplineNode() :
			id(0), p_id(0), type(0), radius(0.f), params() {}

		CompactSplineNode(const NeuronSplineNode& node):
			id(node.id), p_id(node.p_id), type(node.type), radius(node.radius),
			params(CompactSplineParam(node.params))
		{}

		int id;
		int p_id;
		int type;
		float radius;

		struct CompactParam
		{
			VEC3 endPoint;// Only reserve end point.
			int angle_int_x;// A angle to get a tangent.
			int angle_int_y;// A angle to get a tangent.
			int angle_int_z;// A angle to get a tangent.
		} params;
		
		static CompactParam CompactSplineParam(const NCSplineCurve::SCParams& p)
		{
			return CompactParam({
					VEC3(p.XParams.y, p.YParams.y, p.ZParams.y), 
					ReserveAngle(GetAngle(atan(p.XParams.w))),
					ReserveAngle(GetAngle(atan(p.YParams.w))),
					ReserveAngle(GetAngle(atan(p.ZParams.w)))
				});
		}

		static CompactParam CompactSplineParam_SomaBranch(const NCSplineCurve::SCParams& p)
		{
			return CompactParam({
					VEC3(p.XParams.x, p.YParams.x, p.ZParams.x),
					ReserveAngle(GetAngle(atan(p.XParams.z))),
					ReserveAngle(GetAngle(atan(p.YParams.z))),
					ReserveAngle(GetAngle(atan(p.ZParams.z)))
				});
		}

		static NCSplineCurve::SCParams UnCompactSplineParam(const CompactParam& p,
			const CompactParam& parent_p)
		{
			return NCSplineCurve::SCParams({
					VEC4(parent_p.endPoint.x, p.endPoint.x, 
					tan(GetRadian(parent_p.angle_int_x)), tan(GetRadian(p.angle_int_x))),
					VEC4(parent_p.endPoint.y, p.endPoint.y,
					tan(GetRadian(parent_p.angle_int_y)), tan(GetRadian(p.angle_int_y))),
					VEC4(parent_p.endPoint.z, p.endPoint.z,
					tan(GetRadian(parent_p.angle_int_z)), tan(GetRadian(p.angle_int_z))),
				});
		}

		// Save angle(float) with a decimal precision.
		static int ReserveAngle(float angle, int precision=0)
		{
			/*
			Angle can be represent as a int.
			Angle : precision : 1бу: integer part of angle.
				    precision : 0.1бу: integer part of (angle*10).
					...
			*/
			for (int i = 0; i < precision; i++)
			{
				angle *= 10;
			}

			return static_cast<int>(angle);
		}

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

		static Desc& GetCompactSplineDescriptor()
		{
			return GetInstance().neuron_compact_spline_desc;
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
			struct CompactSplineNode{
				int id;
				int p_id;
				int type;
				float radius;

				struct CompactParam
				{
					VEC3 endPoint;// Only reserve end point.
					int angle_int[3];// A angle to get a tangent.
				} params;
			}
			*/
			neuron_compact_spline_desc.clear();
			neuron_compact_spline_desc.push_back(
				new FieldDesc(DataType::INT32, "id", offsetof(CompactSplineNode, id)));
			neuron_compact_spline_desc.push_back(
				new FieldDesc(DataType::INT32, "p_id", offsetof(CompactSplineNode, p_id)));
			neuron_compact_spline_desc.push_back(
				new FieldDesc(DataType::INT32, "type", offsetof(CompactSplineNode, type)));
			neuron_compact_spline_desc.push_back(
				new FieldDesc(DataType::FLOAT, "radius", offsetof(CompactSplineNode, radius)));
			neuron_compact_spline_desc.push_back(
				new FieldDesc(DataType::FLOAT, "params.endPoint.x", offsetof(CompactSplineNode, params.endPoint.x)));
			neuron_compact_spline_desc.push_back(
				new FieldDesc(DataType::FLOAT, "params.endPoint.y", offsetof(CompactSplineNode, params.endPoint.y)));
			neuron_compact_spline_desc.push_back(
				new FieldDesc(DataType::FLOAT, "params.endPoint.z", offsetof(CompactSplineNode, params.endPoint.z)));
			neuron_compact_spline_desc.push_back(
				new FieldDesc(DataType::INT32, "angle_int_x", offsetof(CompactSplineNode, params.angle_int_x)));
			neuron_compact_spline_desc.push_back(
				new FieldDesc(DataType::INT32, "angle_int_y", offsetof(CompactSplineNode, params.angle_int_y)));
			neuron_compact_spline_desc.push_back(
				new FieldDesc(DataType::INT32, "angle_int_z", offsetof(CompactSplineNode, params.angle_int_z)));

		}
	
		Desc neuron_SWC_desc;
		Desc neuron_spline_desc;
		Desc neuron_compact_spline_desc;
	};

}

#endif // !__NC_NEURON_REPRESENTATION_H__

#pragma once
#ifndef _NC_BRANCH_TREE_H_
#define _NC_BRANCH_TREE_H_

#include"spline_curve.h"

/*
Branch Tree : Binary structured tree.
*/

namespace NCNeuron
{
	enum NodeParam
	{
		EMPTY=0,
		SPLINE=1,
		COMPACTSPLINE=2
	};

	template<int>
	struct BranchTree {};

	struct BranchTreeNodeBase
	{
		BranchTreeNodeBase() : 
			l_child(nullptr), r_child(nullptr),
			type(1), radius(1.f)
		{}

		~BranchTreeNodeBase()
		{
			if (l_child) delete l_child;
			if (r_child) delete r_child;
		}

		Branch branch;
		int type;
		float radius;
		BranchTreeNodeBase* l_child;
		BranchTreeNodeBase* r_child;
	};

	/////////////////////////////
	// Branch tree with no param.

	template<>
	struct BranchTree<NodeParam::EMPTY>
	{
		std::vector<BranchTreeNodeBase*> soma_branches;
	};

	//////////////////////////////////
	// Branch Tree with a spline curve.

	struct BranchSplineNode : BranchTreeNodeBase
	{
		NCSplineCurve::SCParams params;
	};

	template<>
	struct BranchTree<NodeParam::SPLINE>
	{
		using NodeType = BranchSplineNode;
		std::vector<BranchSplineNode*> soma_branches;
	};

	// Fit NodeParams into branch tree.
	void FitBranchTree(NCNeuron::BranchTree<NCNeuron::NodeParam::SPLINE>* tree);

	////////////////////////////////////////////
	// Branch Tree with a compact spline param.

	struct BranchCompactSplineNode : BranchSplineNode
	{
		BranchCompactSplineNode():
			BranchSplineNode(), seqeunce_length(0), sequence(nullptr)
		{}

		~BranchCompactSplineNode()
		{
			if (sequence) {
				delete[] sequence;
				sequence = nullptr;
			}
		}
		struct CompactParam
		{
			VEC3 position;

			//int angle_int_x;// A angle to get a tangent.
			//int angle_int_y;// A angle to get a tangent.
			//int angle_int_z;// A angle to get a tangent.
			float angle_int_x;// A angle to get a tangent.
			float angle_int_y;// A angle to get a tangent.
			float angle_int_z;// A angle to get a tangent.
		};
		
		int seqeunce_length;
		CompactParam* sequence;

		static CompactParam CompactSplineParam(
			const NCSplineCurve::SCParams& p)
		{
			/*return CompactParam({
					VEC3(p.XParams.y, p.YParams.y, p.ZParams.y),
					ReserveAngle(GetAngle(atan(p.XParams.w))),
					ReserveAngle(GetAngle(atan(p.YParams.w))),
					ReserveAngle(GetAngle(atan(p.ZParams.w)))
				});*/
			return CompactParam({
					VEC3(p.XParams.y, p.YParams.y, p.ZParams.y),
					p.XParams.w,
					p.YParams.w,
					p.ZParams.w
				});
		}

		// Save angle(float) with a decimal precision.
		static int ReserveAngle(float angle, int precision = 0)
		{
			return static_cast<int>(angle);
		}
	};

	template<>
	struct BranchTree<NodeParam::COMPACTSPLINE>
	{
		using NodeType = BranchCompactSplineNode;
		std::vector<BranchCompactSplineNode*> soma_branches;
	};

	// Fit NodeParams into branch tree.
	void CompactBranchTree(NCNeuron::BranchTree<NCNeuron::NodeParam::COMPACTSPLINE>* tree);
	
	void CompactNode(NCNeuron::BranchCompactSplineNode& node, 
		std::vector<BranchCompactSplineNode::CompactParam>& params);

}

#endif // !_NC_BRANCH_TREE_H_

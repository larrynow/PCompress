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
		std::vector<BranchSplineNode*> soma_branches;
	};

	void FitBranchTree(NCNeuron::BranchTree<NCNeuron::NodeParam::SPLINE>* tree);

}

#endif // !_NC_BRANCH_TREE_H_

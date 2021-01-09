#include "branch_tree.h"
#include "global_setting.h"
#include<queue>
#include<set>
#include<map>
#include"utils.h"
#include"neuron_representation.h"

void NCNeuron::FitBranchTree(NCNeuron::BranchTree<NCNeuron::NodeParam::SPLINE>* tree)
{
	using namespace NCNeuron;

	using NodeT = NCNeuron::BranchSplineNode;

	std::queue<NodeT*> q;
	for(auto& n : tree->soma_branches)
		q.push(n);

	while (!q.empty())
	{
		auto cur = q.front();

		while (true)
		{
			DebugLog("Fit curve");
			FitCurve(cur->branch, cur->params);

			// Check error;
			DebugLog("Check error");
			auto u = CurveError(cur->branch, cur->params);
			if (u.total_error < (GlobalSetting::GetCurveThreshold()*cur->branch.size()) ||
				cur->branch.size() <= 2)
			{
				q.pop();
				if (cur->l_child) q.push(static_cast<NodeT*>(cur->l_child));
				if (cur->r_child) q.push(static_cast<NodeT*>(cur->r_child));
				break;
			}
			else
			{
				// Cut branch into two new.
				auto n_b = new NodeT();
				n_b->l_child = cur->l_child;
				n_b->r_child = cur->r_child;
				cur->l_child = n_b;
				cur->r_child = nullptr;

				n_b->type = cur->type;
				n_b->radius = cur->radius;

				DebugLog("Origin length ", cur->branch.size(), " ", u.max_e_id);
				n_b->branch = cur->branch.Split(u.max_e_id);
				DebugLog("Split into ", cur->branch.size(), " ", n_b->branch.size());

			}
		}
	}
}

void NCNeuron::CompactBranchTree(
	NCNeuron::BranchTree<NCNeuron::NodeParam::COMPACTSPLINE>* tree)
{
	using namespace NCNeuron;

	using NodeT = NCNeuron::BranchCompactSplineNode;
	std::queue<NodeT*> q;
	for (auto& n : tree->soma_branches)
		q.push(n);

	while (!q.empty())
	{
		auto cur = q.front();

		int seqeunce_length = 0;
		std::vector<BranchCompactSplineNode::CompactParam> params;

		DebugLog("Fit curve");
		FitCurve(cur->branch, cur->params);

		// Check error;
		DebugLog("Check error");
		auto u = CurveError(cur->branch, cur->params);
		if (u.total_error < (GlobalSetting::GetCurveThreshold() * cur->branch.size()) ||
			cur->branch.size() <= 2)
		{

		}
		else
		{
			// Create param sequences.
			NodeT nb_1, nb_2;
			nb_2.branch = cur->branch.Split(u.max_e_id);
			nb_1.branch = cur->branch;

			CompactNode(nb_1, params);
			CompactNode(nb_2, params);

			//Log("Params size : ", params.size());

			params.pop_back();// Pop last one.
			int siz = params.size();
			cur->sequence = new BranchCompactSplineNode::CompactParam[siz];
			memcpy(cur->sequence, &params[0], 
				siz*sizeof(BranchCompactSplineNode::CompactParam));
			cur->seqeunce_length = siz;
		}

		q.pop();
		if (cur->l_child) q.push(static_cast<NodeT*>(cur->l_child));
		if (cur->r_child) q.push(static_cast<NodeT*>(cur->r_child));
	}
}

void NCNeuron::CompactNode(NCNeuron::BranchCompactSplineNode& node, 
	std::vector<BranchCompactSplineNode::CompactParam>& params)
{
	NCSplineCurve::FitCurve(node.branch, node.params);
	auto u = NCSplineCurve::CurveError(node.branch, node.params);
	if (u.total_error < (GlobalSetting::GetCurveThreshold() * node.branch.size()) ||
		node.branch.size() <= 2)
	{
		params.push_back(BranchCompactSplineNode::CompactSplineParam(node.params));
		return;
	}
	else
	{
		using NodeT = NCNeuron::BranchCompactSplineNode;

		NodeT nb_1, nb_2;
		nb_2.branch = node.branch.Split(u.max_e_id);
		nb_1.branch = node.branch;

		CompactNode(nb_1, params);
		CompactNode(nb_2, params);

		/*NodeT n_b;
		n_b.branch = node.branch.Split(u.max_e_id);
		CompactNode(node, params);
		CompactNode(n_b, params);*/
	}
}

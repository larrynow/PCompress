#include "branch_tree.h"
#include "global_setting.h"
#include<queue>
#include<set>
#include<map>
#include"utils.h"

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

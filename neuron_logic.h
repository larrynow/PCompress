#pragma once
#ifndef __NC_NEURON_LOGIC_H__
#define __NC_NEURON_LOGIC_H__

#include "neuron_representation.h"
#include <queue>

namespace NCNeuron
{
	// Get neuronTree from a swc file.
	NeuronTree ReadSWC_File(const std::string& file);
	bool SaveSWC(const NeuronTree& n, const std::string& out_file);

	template<int NodeType>
	NCNeuron::BranchTree<NodeType>*
		GetBranchTree(const NCNeuron::NeuronTree& neuron)
	{
		using namespace NCNeuron;

		NCNeuron::BranchTree<NodeType>* ret =
			new NCNeuron::BranchTree<NodeType>();

		//calculate a node's branch num;
		std::vector<int> branches;
		branches = std::vector<int>(neuron.size(), 0);
		for (int i = 0; i < neuron.size(); i++)
		{
			if (neuron[i].parent < 0) {
				continue;//root
			}
			branches[neuron[i].parent - 1]++;
		}

		std::queue<int> q;
		for (int i = 0; i < neuron.size(); i++)
		{
			if (branches[i] == 0)
				q.push(i);
		}

		using nodeT = typename NCNeuron::BranchTree<NodeType>::NodeType;

		std::map<int, int> visited;//Map root to child.
		std::map<int, nodeT*> node_map;//Map id to its node.
		while (!q.empty()) {
			auto current = q.front();
			nodeT* pre = nullptr;

			while (current != 0)
			{
				if (node_map.count(current))
				{
					if (pre != node_map.at(current)->l_child)// From different half.
						node_map.at(current)->r_child = pre;
					break;
				}

				nodeT* node = new nodeT();
				node_map[current] = node;

				node->branch.push_back({ neuron[current].x, neuron[current].y, neuron[current].z });
				node->type = neuron[current].type;
				//Get root in branch.
				auto root = neuron[current].parent - 1;
				for (; branches[root] < 2 && root != 0; root = neuron[root].parent - 1)
				{
					node->branch.push_back({ neuron[root].x, neuron[root].y, neuron[root].z });
				}
				node->branch.push_back({ neuron[root].x, neuron[root].y, neuron[root].z });
				std::reverse(node->branch.begin(), node->branch.end());

				node->l_child = pre;

				if (root == 0) ret->soma_branches.push_back(node);

				current = root;
				pre = node;

			}
			q.pop();
		}

		return ret;
	};

	// From a branch tree get a spline-node vector.
	NeuronSplineTree GetSplineTree(NCNeuron::BranchTree<NCNeuron::NodeParam::SPLINE>* 
		branch_tree);
	// From a spline-node vector get a neuronTree.
	NeuronTree GetNeuronTree(NeuronSplineTree& tree);

	CompactNeuronTree CompactBranchSplineTree(
		NCNeuron::BranchTree<NCNeuron::NodeParam::SPLINE>* branch_tree);

	// From a compact tree.
	NeuronTree GetNeuronTree(CompactNeuronTree& tree);
}

#endif // !__NC_NEURON_LOGIC_H__

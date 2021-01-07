#pragma once
#ifndef __NC_NEURON_LOGIC_H__
#define __NC_NEURON_LOGIC_H__

#include "neuron_representation.h"

namespace NCNeuron
{
	// Get neuronTree from a swc file.
	NeuronTree ReadSWC_File(const std::string& file);
	bool SaveSWC(const NeuronTree& n, const std::string& out_file);

	NeuronTree GetNeuronTree(const NeuronLatentTree&);

	template<int NodeT>
	NCNeuron::BranchTree<NodeT>*
		GetBranchTree(const NCNeuron::NeuronTree& neuron);

	template<>
	NCNeuron::BranchTree<NCNeuron::NodeParam::SPLINE>*
		GetBranchTree(const NCNeuron::NeuronTree& neuron);

	// From a branch tree get a spline-node vector.
	NeuronSplineTree GetSplineTree(NCNeuron::BranchTree<NCNeuron::NodeParam::SPLINE>* 
		branch_tree);

	// From a spline-node vector get a neuronTree.
	NeuronTree GetNeuronTree(NeuronSplineTree& tree);
}

#endif // !__NC_NEURON_LOGIC_H__

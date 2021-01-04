#pragma once
#ifndef __NC_NEURON_LOGIC_H__
#define __NC_NEURON_LOGIC_H__

#include "neuron_representation.h"

namespace NCNeuron
{
	// Get neuronTree from a swc file.
	NeuronTree ReadSWC_File(const std::string& file);
	bool SaveSWC(const NeuronTree& n, const std::string& out_file);

	NeuronTree GetNeuronTree(const NeuronBranchTree&);

}

#endif // !__NC_NEURON_LOGIC_H__

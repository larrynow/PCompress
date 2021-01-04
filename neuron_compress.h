#pragma once
#ifndef _NC_COMPRESS_H_
#define _NC_COMPRESS_H_

#include"neuron_representation.h"

namespace NCNeuron
{
	class NeuronCompressor
	{
	public:
		NeuronCompressor() {}

		// Main compress entrance, choose a level.
		static void Compress(const std::string& neuron_file, int level = 1);

	private:
		static void Level1Compress(const NeuronTree& neuron, const std::string& out_file);
		static void Level2Compress(const NeuronTree& neuron, const std::string& out_file);
		static void Level3Compress(const NeuronTree& neuron, const std::string& out_file);
	};
}

#endif // !_NC_COMPRESS_H_

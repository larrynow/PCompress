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

		// Decompress entrance.
		static NeuronTree* Decompress(const std::string& com_file);

	private:
		static void Level1Compress(const NeuronTree& neuron, const std::string& out_file);
		static void Level2Compress(const NeuronTree& neuron, const std::string& out_file);

		static NeuronTree* Level1Decompress(const std::string& com_file);
		static NeuronTree* Level2Decompress(const std::string& com_file);
	};
}

#endif // !_NC_COMPRESS_H_

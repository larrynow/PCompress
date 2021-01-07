#pragma once
#ifndef _NC_BRANCH_H_
#define _NC_BRANCH_H_

#include<vector>
#include"math.h"
#include<string>
#include"global_setting.h"
#include<fstream>

namespace NCNeuron
{
	using namespace NCMath;

	class Branch
	{
	public:

		void push_back(const VEC3& node)
		{
			sample_points.push_back(node);
		}

		auto begin()
		{
			return sample_points.begin();
		}
		auto end()
		{
			return sample_points.end();
		}

		size_t size() const
		{
			return sample_points.size();
		}

		VEC3& operator [](std::size_t i)
		{
			return sample_points[i];
		}

		void MoveToO()
		{
			VEC3 start_point = sample_points[0];
			for (auto& p : sample_points)
			{
				p = p - start_point;
			}
		}

		const VEC3& StartPoint() const
		{
			return sample_points[0];
		}

		friend std::ostream& operator << (std::ostream& os, const Branch& br)
		{
			os << "Branch display:" << std::endl;
			for (auto& p : br.sample_points)
			{
				os << p << std::endl;
			}

			return os;
		}

		friend bool SaveBranch(const Branch& branch, const std::string& save_file);

		////////////////////////////////////////////
		// Split a branch return a new branch.

		Branch Split(std::size_t pos)
		{
			// Branch:[1,2,3,4,5], pos = 2;
			// Branch:[1,2,3] and return Branch:[3,4,5].

			Branch n_b;
			if (pos < sample_points.size())
			{
				n_b.sample_points = std::vector<VEC3>(
					sample_points.begin() + pos, sample_points.end());// Include pos.

				for (int i = sample_points.size() - 1; i > pos; --i)
					sample_points.pop_back();
			}
			
			return n_b;
		}

	private:
		std::vector<VEC3> sample_points;
	};

	inline bool SaveBranch(const Branch& branch, const std::string& save_file) {
		std::ofstream file(save_file, std::ios::out);
		file.setf(std::ios::fixed);
		file.precision(GlobalSetting::GetDecimalPrecision());
		if (!file.good())
		{
			std::cout << "File ERROR!" << std::endl;
			return false;
		}

		for (int i = 0; i < branch.sample_points.size(); ++i)
		{
			file << i << " " << 2 << " "
				<< branch.sample_points[i].x << " " 
				<< branch.sample_points[i].y << " " 
				<< branch.sample_points[i].z << " " 
				<< 0.5 << " "
				<< i-1 << "\n";
		}

		file.close();
		return true;
	};// BranchInfo->swc.
}

#endif // !_NC_BRANCH_H_

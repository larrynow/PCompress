#include "neuron_logic.h"
#include "file_io.h"
#include<fstream>
#include<sstream>
#include<regex>
#include<stack>
#include"global_setting.h"
#include"utils.h"
#include<unordered_set>

template<class T>
std::vector<T> splitStr(const std::string& in, const std::string& delim)
{
    std::vector<T> ret;
    try
    {
        std::regex re{ delim };
        return std::vector<T>{
            std::sregex_token_iterator(in.begin(), in.end(), re, -1),
                std::sregex_token_iterator()
        };
    }
    catch (const std::exception& e)
    {
        std::cout << "Split error:" << e.what() << std::endl;
    }
    return ret;
}

NCNeuron::NeuronTree NCNeuron::ReadSWC_File(const std::string& filename)
{
    NeuronTree nt;

    std::ifstream file(filename);
    if (!file.good())
    {
        std::cout << "File ERROR!" << std::endl;
        return nt;
    }

    int count = 0;
    std::vector <NeuronSWCNode> listNeuron;
    listNeuron.clear();

    std::string str;// Current string.
    std::stringstream stream;

    while (getline(file, str)) {
        if (str.at(0) == '#')
        {
            continue;
        }

        count++;
        NeuronSWCNode S;

        str.erase(0, str.find_first_not_of(" "));
        auto qsl = splitStr<std::string>(str, " ");
        if (qsl.size() == 0)   continue;

        for (size_t i = 0; i < qsl.size(); i++)
        {
            stream.clear();
            stream << qsl[i];
            if (i == 0)
            {
                stream >> S.n;
            }
            else if (i == 1)
            {
                stream >> S.type;
            }
            else if (i == 2)
            {
                stream >> S.x;
            }
            else if (i == 3)
            {
                stream >> S.y;
            }
            else if (i == 4)
            {
                stream >> S.z;
            }
            else if (i == 5)
            {
                stream >> S.radius;
            }
            else if (i == 6)
            {
                stream >> S.parent;
            }

        }

        listNeuron.push_back(S);
    }

    if (listNeuron.size() < 1)
        return nt;

    return listNeuron;
}

bool NCNeuron::SaveSWC(const NeuronTree & n, const std::string & out_file)
{
	std::ofstream file(out_file, std::ios::out);
	file.setf(std::ios::fixed);
	file.precision(GlobalSetting::GetDecimalPrecision());
	if (!file.good())
	{
		std::cout << "File ERROR!" << std::endl;
		return false;
	}

	for (auto& node : n)
	{
		file << node.n << " " << node.type << " "
			<< node.x << " " << node.y << " " << node.z << " "<<node.radius<<" "
			<< node.parent << "\n";
	}

	file.close();
	return true;
}

NCNeuron::NeuronSplineTree NCNeuron::GetSplineTree(
	NCNeuron::BranchTree<NCNeuron::NodeParam::SPLINE>* branch_tree)
{
	NeuronSplineTree ret_tree;

	std::map<BranchSplineNode*, int> id_map;
	for (auto root : branch_tree->soma_branches)// Start from each soma-branch.
	{
		int pre_id = -1;// Soma.
		std::stack<BranchSplineNode*> s;
		auto cur = root;

		while (cur != nullptr || !s.empty())
		{
			while (cur != nullptr)
			{
				ret_tree.push_back({ static_cast<int>(ret_tree.size()), pre_id, 
					cur->type, cur->radius, cur->params });
				s.push(cur);
				id_map[cur] = ret_tree.size()-1;
				pre_id = ret_tree.size()-1;
				cur = static_cast<BranchSplineNode*>(cur->l_child);
			}
			if (!s.empty())
			{
				BranchSplineNode* top = s.top();
				pre_id = id_map.at(top);
				s.pop();
				cur = static_cast<BranchSplineNode*>(top->r_child);
			}
		}
	}

	return ret_tree;
}

NCNeuron::NeuronTree NCNeuron::GetNeuronTree(NeuronSplineTree& tree)
{
	NCNeuron::NeuronTree ret_tree;

	int cur_id = 1;
	int parent_id = -1;
	// Put soma.
	VEC3 soma;
	ret_tree.push_back({ cur_id++, 1, 0.f, 0.f, 0.f, 1.f, parent_id});

	std::map<int, int> node_last_id_map;// Mark last sample-point-id of a node.
	for (auto& node : tree)
	{
		if (node.p_id == -1) parent_id = 1;
		else parent_id = node_last_id_map.at(node.p_id);
		Branch temp;
		auto sample_num = static_cast<int>(
			GetBranchLength(node.params) / GlobalSetting::GetSamplePointStep());
		sample_num = std::max(sample_num, 3);
		NCSplineCurve::RemakeCurve(node.params, temp, sample_num);
		
		if (node.p_id == -1) soma = temp[0];

		// From second point.
		for (std::size_t i = 1; i < temp.size(); i++)
		{
			auto& point = temp[i];
			//auto& point = temp[i] + transform;
			ret_tree.push_back({cur_id, node.type, 
				point.x, point.y, point.z, node.radius,  parent_id });
			parent_id = cur_id++;
		}
		node_last_id_map[node.id] = cur_id - 1;
	}

	ret_tree[0].x = soma.x;
	ret_tree[0].y = soma.y;
	ret_tree[0].z = soma.z;

	return ret_tree;
}

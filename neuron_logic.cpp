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
		auto& start_point = temp[0];// Start point overlap with prev branch's last point.
		auto target_point = VEC3(ret_tree[parent_id-1].x, 
			ret_tree[parent_id-1].y, ret_tree[parent_id-1].z);
		auto transform = target_point - start_point;

		// From second point.
		for (std::size_t i = 1; i < temp.size(); i++)
		{
			auto& point = temp[i] + transform;
			ret_tree.push_back({cur_id, node.type, 
				point.x, point.y, point.z, node.radius,  parent_id });
			parent_id = cur_id++;
		}
		node_last_id_map[node.id] = cur_id - 1;
	}

	return ret_tree;
}

NCNeuron::NeuronCompactSplineTree NCNeuron::GetCompactSplineTree(
	NCNeuron::BranchTree<NCNeuron::NodeParam::COMPACTSPLINE>* branch_tree)
{
	NeuronCompactSplineTree ret_tree;

	std::map<BranchCompactSplineNode*, int> id_map;
	for (auto root : branch_tree->soma_branches)// Start from each soma-branch.
	{
		int pre_id = -1;// Soma.
		std::stack<BranchCompactSplineNode*> s;
		auto cur = root;

		while (cur != nullptr || !s.empty())
		{
			while (cur != nullptr)
			{
				ret_tree.push_back({ static_cast<int>(ret_tree.size()), pre_id,
					cur->type, cur->radius, cur->params, 
					cur->seqeunce_length, cur->sequence });
				s.push(cur);
				id_map[cur] = ret_tree.size() - 1;
				pre_id = ret_tree.size() - 1;
				cur = static_cast<BranchCompactSplineNode*>(cur->l_child);
			}
			if (!s.empty())
			{
				BranchCompactSplineNode* top = s.top();
				pre_id = id_map.at(top);
				s.pop();
				cur = static_cast<BranchCompactSplineNode*>(top->r_child);
			}
		}
	}

	return ret_tree;
}

NCNeuron::NeuronTree NCNeuron::GetNeuronTree(NCNeuron::NeuronCompactSplineTree& tree)
{
	NCNeuron::NeuronTree ret_tree;

	int cur_id = 1;
	int parent_id = -1;
	// Put soma.
	ret_tree.push_back({ cur_id++, 1, 0.f, 0.f, 0.f, 1.f, parent_id });

	std::map<int, int> node_last_id_map;// Mark last sample-point-id of a node.
	for (auto& node : tree)
	{
		if (node.p_id == -1) parent_id = 1;
		else parent_id = node_last_id_map.at(node.p_id);

		if (node.param_size == 0)
		{// Without any compact param. Directly create a branch.
			Branch temp;
			auto sample_num = static_cast<int>(
				GetBranchLength(node.params) / GlobalSetting::GetSamplePointStep());
			sample_num = std::max(sample_num, 3);
			NCSplineCurve::RemakeCurve(node.params, temp, sample_num);

			// Transform.
			auto& start_point = temp[0];// Start point overlap with prev branch's last point.
			auto target_point = VEC3(ret_tree[parent_id - 1].x,
				ret_tree[parent_id - 1].y, ret_tree[parent_id - 1].z);
			auto transform = target_point - start_point;

			// From second point.
			for (std::size_t i = 1; i < temp.size(); i++)
			{
				auto& point = temp[i] + transform;
				ret_tree.push_back({ cur_id, node.type,
					point.x, point.y, point.z, node.radius,  parent_id });
				parent_id = cur_id++;
			}
			node_last_id_map[node.id] = cur_id - 1;
		}
		else
		{
			auto last_point = VEC3(node.params.XParams.x,
				node.params.YParams.x, node.params.ZParams.x
			);
			auto last_tangent = VEC3(node.params.XParams.z,
				node.params.YParams.z, node.params.ZParams.z
			);
			for (int i = 0; i < node.param_size; i++)
			{
				auto param_point = node.param_sequences[i].position;
				/*VEC3 tangent = VEC3(
					tan(GetRadian(node.param_sequences[i].angle_int_x)),
					tan(GetRadian(node.param_sequences[i].angle_int_y)),
					tan(GetRadian(node.param_sequences[i].angle_int_z))
				);*/
				VEC3 tangent = VEC3(
					node.param_sequences[i].angle_int_x,
					node.param_sequences[i].angle_int_y,
					node.param_sequences[i].angle_int_z
				);

				NCSplineCurve::SCParams p;
				p.XParams = VEC4(last_point.x, param_point.x, last_tangent.x, tangent.x);
				p.YParams = VEC4(last_point.y, param_point.y, last_tangent.y, tangent.y);
				p.ZParams = VEC4(last_point.z, param_point.z, last_tangent.z, tangent.z);

				Branch temp;

				auto length = param_point.SqrDistance(last_point);
				if (length > 0.f) length = std::sqrt(length);
				else length = 0.f;
				auto sample_num = static_cast<int>(
					length / GlobalSetting::GetSamplePointStep());
				sample_num = std::max(sample_num, 3);
				NCSplineCurve::RemakeCurve(p, temp, sample_num);

				// Move.
				auto& start_point = temp[0];// Start point overlap with prev branch's last point.
				auto target_point = VEC3(ret_tree[parent_id - 1].x,
					ret_tree[parent_id - 1].y, ret_tree[parent_id - 1].z);
				auto transform = target_point - start_point;

				// From second point.
				for (std::size_t j = 1; j < temp.size(); j++)
				{
					auto& point = temp[j] + transform;
					ret_tree.push_back({ cur_id, node.type,
						point.x, point.y, point.z, node.radius,  parent_id });
					parent_id = cur_id++;
				}

				last_point = param_point;
				last_tangent = tangent;
			}
			
			/////////////////////////
			// For last branch.
			NCSplineCurve::SCParams p = node.params;
			p.XParams.x = last_point.x; 
			p.YParams.x = last_point.y;
			p.ZParams.x = last_point.z;

			p.XParams.z = last_tangent.x;
			p.YParams.z = last_tangent.y;
			p.ZParams.z = last_tangent.z;

			Branch temp;
			VEC3 point = VEC3(node.params.XParams.y,
				node.params.YParams.y,
				node.params.ZParams.y
				);
			auto length = point.SqrDistance(last_point);
			if (length >= 0.0) length = std::sqrt(length);
			else length = 0.0;
			auto sample_num = static_cast<int>(
				length / GlobalSetting::GetSamplePointStep());
			sample_num = std::max(sample_num, 3);
			NCSplineCurve::RemakeCurve(p, temp, sample_num);

			// Move.
			auto& start_point = temp[0];// Start point overlap with prev branch's last point.
			auto target_point = VEC3(ret_tree[parent_id - 1].x,
				ret_tree[parent_id - 1].y, ret_tree[parent_id - 1].z);
			auto transform = target_point - start_point;

			// From second point.
			for (std::size_t i = 1; i < temp.size(); i++)
			{
				auto& point = temp[i] + transform;
				ret_tree.push_back({ cur_id, node.type,
					point.x, point.y, point.z, node.radius,  parent_id });
				parent_id = cur_id++;
			}

			node_last_id_map[node.id] = cur_id - 1;
		}
		
	}

	return ret_tree;
}


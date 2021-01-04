#include "neuron_logic.h"
#include "file_io.h"
#include<fstream>
#include<sstream>
#include<regex>

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

        for (int i = 0; i < qsl.size(); i++)
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

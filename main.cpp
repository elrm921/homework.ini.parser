#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>

using namespace std;
typedef pair<string,string> kvalue;
typedef pair<string,vector<kvalue>> vdata;

class ini_parser
{
    string src;
    vector<vdata> m_data;
public:
    ini_parser(const string& name) : src(name)
    {
        vector<vdata>::iterator current_section;
        ifstream source_data(src);
        if (!source_data.is_open()) throw logic_error("No such file " + src);
        while (source_data.is_open() && !source_data.eof())
        {
            string line;
            getline(source_data, line);
            size_t end = line.find(';',0);
            line = line.substr(0,end);

            regex re_section("\\[\\w+\\]", regex::ECMAScript|regex::icase);
            if (regex_search(line, re_section))
            {
                line = line.substr(1,line.size()-2);
                if (find_data_section(m_data, line) == m_data.end())
                {
                    vdata section;
                    section.first = line;
                    m_data.push_back(section);
                }
                current_section = find_data_section(m_data, line);
            }

            regex re_key_value("^.+\\=.*$", regex::ECMAScript|regex::icase);
            if (regex_search(line, re_key_value))
            {
                size_t split = line.find('=',0);
                string key = line.substr(0,split);
                string value = line.substr(split+1);

                kvalue this_pair(key,value);
                auto it = find_section_key(current_section->second, key);
                if (it == current_section->second.end())
                    current_section->second.push_back(this_pair);
                else
                    it->second = value;
            }
        }
    }

    template<typename T>
    T get_value(string str)
    {
        size_t split = str.find('.',0);
        string section = str.substr(0,split);
        string skey = str.substr(split+1);
        auto it1 = find_data_section(m_data,section);
        if (it1 == m_data.end()) throw logic_error("Section " + section + " does not exist");
        vector<kvalue> keys = it1->second;
        auto it2 = find_section_key(keys,skey);
        if (it2 == keys.end()) throw logic_error("Key " + skey + " does not exist");
        T res = it2->second;
        return res;
    }

    vector<vdata>::iterator find_data_section(vector<vdata> &data, string &name)
    {
        auto it = data.begin();
        while (it != data.end())
        {
            if (it->first == name)
                break;
            it++;
        }
        return it;
    }

    vector<kvalue>::iterator find_section_key(vector<kvalue> &section, string &key)
    {
        auto it = section.begin();
        while (it != section.end())
        {
            if (it->first == key)
                break;
            it++;
        }
        return it; 
    }

    void print_data()
    {
        for (auto &p : m_data)
        {
            cout << p.first << endl;
            for (auto &v : p.second)
            {
                cout << v.first << "=" << v.second << endl;
            }
        }
    }
};

int main()
{
    try
    {
        ini_parser parser("example.ini");
        // parser.print_data();
        auto value = parser.get_value<string>("Section1.var1");
        cout << "get_value::section1::var1 " << value << endl;
    }
    catch(const logic_error& e)
    {
        std::cerr << e.what() << endl;
    }
}
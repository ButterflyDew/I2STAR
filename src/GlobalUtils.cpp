#include "../include/GlobalUtils.hpp"
#include "../include/Graph.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

int index_offset = 0;
fs::path fs_filesystem = "data/LinkedMDB/";


// 读取询问文件，返回所有询问集合
vector<vector<vector<int>>> read_query_file() 
{
    vector<vector<vector<int>>> all_queries;
    ifstream fin((fs_filesystem / "query.txt").string());
    if (!fin.is_open()) throw runtime_error("Query file open failed");
    int q; fin >> q;
    for (int i = 0; i < q; ++i) 
    {
        int g; fin >> g;
        vector<vector<int>> query;
        for (int j = 0; j < g; ++j) 
        {
            int s; fin >> s;
            vector<int> group(s);
            for (int k = 0; k < s; ++k) 
            {
                fin >> group[k];
                group[k] += index_offset;
            }
            query.push_back(group);
        }
        all_queries.push_back(query);
    }
    return all_queries;
} 
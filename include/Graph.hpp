#pragma once
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "GlobalUtils.hpp"
using namespace std;
namespace fs = std::filesystem;

// 存图类模板，支持无向带权图，边权类型可变
// 节点编号 1-index，adj 存储邻接表，edge_map 存储点对到边权

template<typename edgetype>
class Graph {
public:
    Graph();
    int get_n() const { return n; }
    int get_m() const { return m; }
    const vector<vector<pair<int, edgetype>>>& get_adj() const { return adj; }
    const map<pair<int, int>, edgetype>& get_edge_map() const { return edge_map; }
    int Find(int x) { return f[x] == x ? x : f[x] = Find(f[x]); }
    void Union(int x, int y) { f[Find(x)] = Find(y); }
    // 其他方法如：加边、查边等
private:
    int n, m;
    vector<vector<pair<int, edgetype>>> adj; // 邻接表
    map<pair<int, int>, edgetype> edge_map;  // 点对到边权
    vector<int> f;
};

// 模板实现
extern int index_offset;
template<typename edgetype>
Graph<edgetype>::Graph() {
    ifstream fin((fs_filesystem / "graph.txt").string());
    if (!fin.is_open()) throw runtime_error("Graph file open failed");
    fin >> n >> m;
    adj.assign(n + 1, vector<pair<int, edgetype>>());
    f.assign(n + 1, 0);
    for(int i = 1; i <= n; i++)
        f[i] = i;

    for (int i = 0; i < m; ++i) {
        int u, v;
        edgetype w;
        fin >> u >> v >> w;
        u += index_offset;
        v += index_offset;
        adj[u].emplace_back(v, w);
        adj[v].emplace_back(u, w);
        edge_map[{min(u, v), max(u, v)}] = w;
        Union(u, v);
    }

    int cnt = 0;
    for(int i = 1; i <= n; i++)
        if(Find(i) == i)
            cnt++;
    Log::info("Graph has " + to_string(cnt) + " connected components");
} 
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

namespace fast {
    static char B[1 << 18], *S = B, *T = B;
    #define getc_fast() (S == T && \
        (T = (S = B) + fread(B, 1, 1 << 18, stdin), S == T) \
        ? 0 : *S++)
    inline int read() {
        int x = 0, c;
        // 跳过非数字
        while ((c = getc_fast()) && (c < '0' || c > '9'));
        // 读数字
        for (; c >= '0' && c <= '9'; c = getc_fast())
            x = x * 10 + (c - '0');
        return x;
    }
    inline double readDouble() {
        double x = 0.0;
        int c;
        while ((c = getc_fast()) && (c < '0' || c > '9') && c != '-' && c != '.');
        bool neg = false;
        if (c == '-') {
            neg = true;
            c = getc_fast();
        }
        // 整数部分
        for (; c >= '0' && c <= '9'; c = getc_fast())
            x = x * 10 + (c - '0');
        // 小数部分
        if (c == '.') {
            double div = 1.0;
            c = getc_fast();
            for (; c >= '0' && c <= '9'; c = getc_fast()) {
                x = x * 10 + (c - '0');
                div *= 10.0;
            }
            x /= div;
        }
        return neg ? -x : x;
    }
}
using fast::read;
using fast::readDouble;


template<typename edgetype>
class Graph {
public:
    Graph();
    int get_n() const { return n; }
    int get_m() const { return m; }
    const vector<vector<pair<int, edgetype>>>& get_adj() const { return adj; }
    // const map<pair<int, int>, edgetype>& get_edge_map() const { return edge_map; }
    int Find(int x) { return f[x] == x ? x : f[x] = Find(f[x]); }
    void Union(int x, int y) { f[Find(x)] = Find(y); }
    edgetype get_max_weight() const { return max_weight; }
    edgetype get_min_weight() const { return min_weight; }
    // 其他方法如：加边、查边等
private:
    int n, m;
    vector<vector<pair<int, edgetype>>> adj; // 邻接表
    // map<pair<int, int>, edgetype> edge_map;  // 点对到边权
    edgetype max_weight, min_weight;
    vector<int> f;
};

// 模板实现
extern int index_offset;
template<typename edgetype>
Graph<edgetype>::Graph() {

    Timer::start("read_graph");

    // 把 graph.txt 重定向到 stdin
    if (!freopen((fs_filesystem / "graph.txt").string().c_str(), "r", stdin)) {
        throw runtime_error("Graph file open failed");
    }
    
    n = read(), m = read();

    Log::info("m : " + to_string(m));

    adj.assign(n + 1, vector<pair<int, edgetype>>());
    f.assign(n + 1, 0);
    for(int i = 1; i <= n; i++) f[i] = i;
    
    int cnt0 = 0;
    
    max_weight = 0, min_weight = numeric_limits<edgetype>::max();
    
    for (int i = 0; i < m; ++i) {
        int u, v;
        edgetype w;
        u = read(), v = read(), w = readDouble();
        if(w != 0)
        {
            max_weight = max(max_weight, w);
            min_weight = min(min_weight, w);
        }
        u += index_offset;
        v += index_offset;
        adj[u].emplace_back(v, w);
        adj[v].emplace_back(u, w);
        // edge_map[{min(u, v), max(u, v)}] = w;
        if(w == 0) cnt0++;
        Union(u, v);
    }
    
    int cnt = 0;
    for(int i = 1; i <= n; i++) 
        if(Find(i) == i) 
            cnt++;
    Log::info("Graph has " + to_string(cnt) + " connected components");
    Log::info("Graph has " + to_string(cnt0) + " edges with weight 0");
    Log::info("max_weight: " + to_string(max_weight) + " min_weight: " + to_string(min_weight));

    Timer::stop("read_graph", LogLevel::LOG_INFO);
} 
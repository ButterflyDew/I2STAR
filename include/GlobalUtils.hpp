#pragma once
#include <bits/stdc++.h>
#include <filesystem>
#include "Log.hpp"
using namespace std;
namespace fs = std::filesystem;

// 全局参数
extern int index_offset;      // 0 或 1，表示编号偏移
extern fs::path fs_filesystem;   // 数据目录

// Timer 工具类
#include <chrono>
class Timer {
public:
    // 开始计时
    static void start(const std::string& name) {
        timers[name] = std::chrono::high_resolution_clock::now();
    }
    // 结束计时并输出到日志
    static double stop(const std::string& name, LogLevel level = LogLevel::LOG_DEBUG, bool print = true) {
        auto end = std::chrono::high_resolution_clock::now();
        auto it = timers.find(name);
        if (it != timers.end()) {
            auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - it->second).count();
            double duration_s = duration_us / 1000000.0;
            std::ostringstream oss;
            oss << "[Timer] " << name << ": " << duration_s << " s"; 
            if(print) Log::log(level, oss.str());
            timers.erase(it);
            return duration_s;
        } else {
            Log::log(level, "[Timer] " + name + ": not started!");
            return -1;
        }
    }
private:
    static inline std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timers;
};

// Tree 模板类，支持添加无向带权边

template<typename edgetype>
class Tree {
public:
    Tree() : sum_weight(0) {}
    Tree(edgetype sum_weight) : sum_weight(sum_weight) {}

    void Print() const {
        for(auto [uv, w] : edges) {
            Log::log(LogLevel::LOG_INFO, to_string(uv.first) + " " + to_string(uv.second) + " " + to_string(w));
        }
        Log::log(LogLevel::LOG_INFO, "sum_weight: " + to_string(sum_weight));
    }
    void add_edge(int u, int v, edgetype w) {
        if(u > v) swap(u, v);
        if(edges.find({u, v}) != edges.end())
        {
            sum_weight -= edges[{u, v}];
            edges[{u, v}] = min(edges[{u, v}], w);
        }
        else
            edges[{u, v}] = w;
        sum_weight += edges[{u, v}];
    }
    void delete_edge(int u, int v)
    {
        if(u > v) swap(u, v);
        if(edges.find({u, v}) != edges.end())
        {
            sum_weight -= edges[{u, v}];
            edges.erase({u, v});
        }
    }
    const map<pair<int, int>, edgetype>& get_edges() const { return edges; }
    int get_size() const { return edges.size(); }
    edgetype get_sum_weight() const { return sum_weight; }
private:
    edgetype sum_weight;
    map<pair<int, int>, edgetype> edges;
};

// 读取图文件
void read_graph_file();
// 读取询问文件，返回所有询问集合
vector<vector<vector<int>>> read_query_file(); 
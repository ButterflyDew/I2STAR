#pragma once
#include "Graph.hpp"
#include "GlobalUtils.hpp"
#include "BinaryHeap.hpp"
#include "StaticHeap.hpp"
#include "Log.hpp"
#include <vector>
using namespace std;

// GSTCover 作为全局方法

template<typename edgetype>
Tree<edgetype> gst_cover(const Graph<edgetype>& graph, vector<vector<int>>& query) {
    Log::debug("GSTCover start");
    
    
    edgetype INF = numeric_limits<edgetype>::max();
    Tree<edgetype> answer(INF);

    int g = query.size();
    if(g == 1) return Tree<edgetype>(0);
    for(int i = 1; i < g; i++)
        if(query[i].size() < query[0].size())
            swap(query[i], query[0]);

    if(query[0].size() == 0) return answer;
    
    Log::debug("g[0] size: " + to_string(query[0].size()));

    int n = graph.get_n();
    vector <vector <edgetype> > dist(g, vector <edgetype> (n + 1, INF));
    vector <vector <int> > prev(g, vector <int> (n + 1, -1));
    vector<vector<int>> LS(n + 1, vector<int>(g));
    for (auto& v : LS) iota(v.begin(), v.end(), 0);
    auto calc_dist = [&]
    {
        vector<pair<edgetype, int>> dist_vec(n + 1);
        
        for(int ki = 0; ki < g; ki++)
        {
            for(auto x : query[ki])
                dist[ki][x] = 0;
                
            for (int i = 1; i <= n; ++i)
                dist_vec[i]={dist[ki][i], i};
            StaticHeap<pair<edgetype, int>> heap(dist_vec);
            while(!heap.empty())
            {
                auto [d, u] = heap.top();
                heap.pop();
                for(auto [v, w] : graph.get_adj()[u])
                {
                    if(dist[ki][v] > dist[ki][u] + w)
                        dist[ki][v] = dist[ki][u] + w, prev[ki][v] = u, heap.modify(v, {dist[ki][v], v});
                }
            }
        }
    };

    calc_dist();
    Log::debug("calc_dist done");

    vector <pair <edgetype, int> > pres_info(n + 1);
    for(int c = 1; c <= n; c++)
    {
        sort(LS[c].begin() + 1, LS[c].end(), [&](int i, int j){return dist[i][c] < dist[j][c];});
        edgetype sum = dist[0][c], mi = INF;
        int mi_pres = 0;
        for(int i = 1; i < g; i++)
        {
            sum += dist[LS[c][i]][c];
            if(sum/i < mi)
            {
                mi = sum / i;
                mi_pres = i;
            }
        }
        pres_info[c] = {mi, mi_pres};
    }    

    Log::debug("pres_info done");

    //for delete leave
    vector <vector<pair<int, edgetype>> > ed(n + 1);
    vector <int> deg(n + 1, 0);
    vector <vector <int> > cover_groups(n + 1);
    for(int i = 0; i < g; i++)
        for(auto x : query[i])
            cover_groups[x].push_back(i);

    for(auto r: query[0])
    {
        Log::debug("r: " + to_string(r));
        Tree<edgetype> now_answer;
                
        vector <edgetype> dis(n + 1, INF);
        dis[r] = 0;
        vector <int> pre(n + 1, -1);
        
        BinaryHeap<pair<edgetype, int>> q(n);
        q.push({0, r});
        auto run_dijkstra = [&](int c)
        {
            while(!q.empty())
            {
                auto [d, u] = q.top();
                if(u == c) return;
                q.pop();
                for(auto [v, w] : graph.get_adj()[u])
                {
                    if(dis[v] > dis[u] + w)
                        dis[v] = dis[u] + w, pre[v] = u, q.push({dis[v], v});
                }
            }
        };


        struct node
        {
            edgetype val;
            int pres;
            int c, isreal;
            node() : val(), pres(0), c(0), isreal(0) {}
            node(edgetype val, int pres, int c, int isreal) : val(val), pres(pres), c(c), isreal(isreal) {}
            bool operator < (const node& other) const
            {
                return val < other.val;
            }
        };

        vector <node> nodes(n + 1);
        vector <int> pres_cnt(n + 1, 0);
        for(int c = 1; c <= n; c++)
        {
            nodes[c] = node(pres_info[c].first, pres_info[c].second, c, 0);
            pres_cnt[c] = pres_info[c].second;
        }
        
        StaticHeap <node> heap(nodes);
        vector <int> is_cover(g, 0);
        is_cover[0] = 1;
        int current_cover = 1;

        auto add_edge = [&](int u, vector <int> &par, vector <edgetype> &weight)
        {
            while(par[u] != -1)
            {
                now_answer.add_edge(u, par[u], weight[u] - weight[par[u]]);
                if(now_answer.get_sum_weight() >= answer.get_sum_weight())
                    return false;
                u = par[u];
            }   
            return true; 
        };

        while(current_cover < g)
        {
            Log::debug("current_cover: " + to_string(current_cover));
            if(heap.empty())
                break;
            auto [val, pres, c, isreal] = heap.top();
            Log::debug("c: " + to_string(c) + " val: " + to_string(val) + " pres: " + to_string(pres) + " isreal: " + to_string(isreal));
            int cover_cnt = 0;
            for(int i = 1; i <= pres; i++) if(is_cover[LS[c][i]]==0)
                cover_cnt++;
            Log::debug("Is cover all: " + std::string((cover_cnt == pres_cnt[c]) ? "1" : "0"));
            if(cover_cnt == pres_cnt[c])
            {
                if(isreal)
                {
                    if(!add_edge(c, pre, dis))
                        break;
                    bool flag = 0;
                    for(int i = 1; i <= pres; i++)
                    {
                        if(is_cover[LS[c][i]]) continue;
                        if(!add_edge(c, prev[LS[c][i]], dist[LS[c][i]]))
                        {
                            flag = 1;
                            break;
                        }
                        is_cover[LS[c][i]] = 1;
                        current_cover++;
                    }
                    if(flag) break;
                }   
                else
                {
                    if(pre[c] == -1)
                        run_dijkstra(c);
                    Log::debug("run_dijkstra done");
                    nodes[c].isreal = 1;
                }
            }
            edgetype sum = nodes[c].isreal ? dis[c] : dist[LS[c][0]][c], mi = INF;
            nodes[c].pres = 0;
            pres_cnt[c] = 0;
            int pres_cnt_now = 0;
            Log::debug("Before updata");
            for(int i = 1; i < g; i++)
            {
                if(is_cover[LS[c][i]]) continue;
                sum += dist[LS[c][i]][c];
                pres_cnt_now++;
                if(sum/pres_cnt_now < mi)
                {
                    mi = sum / pres_cnt_now;
                    nodes[c].val = mi;
                    nodes[c].pres = i;
                    pres_cnt[c] = pres_cnt_now;
                }
            }
            Log::debug("Before modify");
            if(nodes[c].pres != 0)
                heap.modify(c, nodes[c]);
            else
                heap.pop();
        }

        auto delete_leave = [&]()
        {
            set <int> vex;
            for(auto [uv, w]: now_answer.get_edges())
            {
                auto [u, v] = uv;
                vex.insert(u), vex.insert(v);
                deg[u]++, deg[v]++;
                ed[u].push_back({v, w});
                ed[v].push_back({u, w});
            }
            vector <int> cover_cnt(g, 0);
            for(auto x : vex)
                for(auto y : cover_groups[x])
                    cover_cnt[y]++;
            auto ck = [&](int x)
            {
                for(auto y : cover_groups[x])
                    if(cover_cnt[y] == 1)
                        return false;
                for(auto y: cover_groups[x])
                    cover_cnt[y]--;
                return true;
            };
            queue <int> q;
            for(auto x : vex)
                if(deg[x] == 1 && ck(x))
                    q.push(x);
            while(!q.empty())
            {
                int u = q.front();
                q.pop();
                for(auto [v, w] : ed[u])
                {
                    deg[v]--;
                    now_answer.delete_edge(u, v);
                    if(deg[v] == 1 && ck(v))
                        q.push(v);
                }
            }

            for(auto x : vex)
            {
                deg[x] = 0;
                ed[x].clear();
            }
        };  
        delete_leave();

        Log::debug("weight: " + to_string(now_answer.get_sum_weight()));
        Log::debug("current_cover: " + to_string(current_cover));
        if(current_cover == g && now_answer.get_sum_weight() < answer.get_sum_weight())
            answer = now_answer;
    }

    //answer.Print();
    return answer;
} 
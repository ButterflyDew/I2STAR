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
Tree<edgetype> gst_cover_v2(const Graph<edgetype>& graph, vector<vector<int>>& query) {
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
    //Log::info("n: " + to_string(n) + " g: " + to_string(g));

    vector <int> cover_weight(n + 1, 0);
    for(int i = 0; i < g; i++)
    {
        for(auto x : query[i])
            cover_weight[x] ++;
    }
    sort(query[0].begin(), query[0].end(), [&](int i, int j){return cover_weight[i] > cover_weight[j];});
    if(cover_weight[query[0][0]] == g) return Tree<edgetype>(0);

    vector <vector <edgetype> > dist(g, vector <edgetype> (n + 1, INF));
    vector <vector <int> > prev(g, vector <int> (n + 1, -1));
    vector<vector<int>> LS(n + 1, vector<int>(g));
    for (auto& v : LS) iota(v.begin(), v.end(), 0);
    
    edgetype max_dis = 0, D = graph.get_min_weight();
    vector <vector <pair <edgetype, int> > > parti_queue(n + 1);
    int queue_cur = 0;
    auto reset_queue = [&]()
    {
        queue_cur = 0;
    };
    auto enqueue = [&](int u, edgetype d)
    {
        if(d > max_dis) max_dis = d;
        int id = d / D;
        if((int)parti_queue.size() <= id) parti_queue.resize(id + 1);
        parti_queue[id].push_back({d, u});
    };
    auto parti_empty = [&]()
    {
        while(queue_cur < (int)parti_queue.size() && parti_queue[queue_cur].empty()) queue_cur++;
        return queue_cur == (int)parti_queue.size();
    };
    auto dequeue = [&]()
    {
        while(queue_cur < (int)parti_queue.size() && parti_queue[queue_cur].empty()) queue_cur++;
        auto [d, u] = parti_queue[queue_cur].back();
        parti_queue[queue_cur].pop_back();
        return pair<edgetype, int>{d, u};
    };
    
    auto calc_dist = [&]
    {
        for(int ki = 0; ki < g; ki++)
        {
            for(auto x : query[ki])
            dist[ki][x] = 0, enqueue(x, 0);
            while(!parti_empty())
            {
                auto [d, u] = dequeue();
                max_dis = max(max_dis, d);
                for(auto [v, w] : graph.get_adj()[u])
                {
                    if(dist[ki][v] > dist[ki][u] + w)
                    dist[ki][v] = dist[ki][u] + w, prev[ki][v] = u, enqueue(v, dist[ki][v]);
                }
            }
            reset_queue();
        }
    };
    
    Timer::start("calc_dist");
    calc_dist();
    Log::debug("calc_dist done");
    Timer::stop("calc_dist", LogLevel::LOG_INFO, 0);
    
    vector<vector<int>> rk(n + 1, vector<int>(g));
    for(int c = 1; c <= n; c++)
    {
        sort(LS[c].begin() + 1, LS[c].end(), [&](int i, int j){return dist[i][c] < dist[j][c];});
        for(int j = 1; j <= g - 1; j++) {
            rk[c][LS[c][j]] = j;
        }
    }    

    Log::debug("pres_info done");

    //for delete leave
    vector <vector<pair<int, edgetype>> > ed(n + 1);
    vector <int> deg(n + 1, 0);
    vector <vector <int> > cover_groups(n + 1);
    for(int i = 0; i < g; i++)
        for(auto x : query[i])
            cover_groups[x].push_back(i);

    Timer::start("MAIN");

    // Log::debug("g : " + to_string(g));

    edgetype min_full_cover = INF;
    int min_full_cover_v = 0;
    auto calc_min_full_cover = [&]()
    {
        for(int i = 1; i <= n; i++) 
        {
            edgetype sum = 0;
            for(int j = 0; j <= g - 1; j++) 
                sum += dist[LS[i][j]][i];
            if(sum / (g - 1) < min_full_cover)
            {
                min_full_cover = sum / (g - 1);
                min_full_cover_v = i;
            }
        }
        // Log::info("min_full_cover: " + to_string(min_full_cover));
    };
    calc_min_full_cover();

    vector <pair<edgetype, int> > cost_list(1);
    vector <vector<edgetype> > dis_allowed_mx;
    int start_cur = 0;
    auto calc_dis_allowed_mx = [&]()
    {
        for(int i = 1; i <= n; i++) 
            for(int j = 1; j <= g - 1; j++)
            {
                if(dist[LS[i][j]][i] > min_full_cover*(g - 1))
                    break;
                cost_list.push_back({dist[LS[i][j]][i], i});
            }
        sort(cost_list.begin() + 1, cost_list.end());
        int len = cost_list.size();
        dis_allowed_mx.resize(len);

        vector <priority_queue <pair<edgetype, int >, vector<pair<edgetype, int>>, greater<pair<edgetype, int>> > > pq(g);
        vector <pair<edgetype, int> > pre_info(n + 1, {0, 0});
        auto get_now_mi = [&]()
        {
            vector <edgetype> now_mi(g, INF);
            for(int i = 1; i <= g - 1; i++)
            {
                while(!pq[i].empty() && pre_info[pq[i].top().second].second > i)
                    pq[i].pop();
                if(!pq[i].empty())
                    now_mi[i] = pq[i].top().first;
            }
            return now_mi;
        };
        for(int i = 1; i < len; i++)
        {
            auto [cost, u] = cost_list[i];
            pre_info[u].first += cost;
            pre_info[u].second++;
            pq[pre_info[u].second].push({pre_info[u].first, u});
            dis_allowed_mx[i] = get_now_mi();
            if(cost < min_full_cover)
                start_cur = i;
        }
    };
    calc_dis_allowed_mx();

    double average_point_ratio_0 = 0, average_point_ratio_all = 0;
    vector<int> dijk_rk(n + 1, 0);
    double dijk_rk_sum = 0;
    int dijk_rk_cnt = 0;

    edgetype now_min_ans = INF;
    
    auto add_edge = [&](int u, vector <int> &par, vector <edgetype> &weight, Tree<edgetype> &now_answer)
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

    for(auto r: query[0])
    {
        Tree<edgetype> now_answer;
                
        vector <edgetype> dis(n + 1, INF);
        dis[r] = 0;
        vector <int> pre(n + 1, -1);
        priority_queue <pair<edgetype, int>, vector<pair<edgetype, int>>, greater<pair<edgetype, int>>> pq;
        pq.push({0, r});
        vector <int> useful_points;

        vector<int> best(n + 1, g - 1);
        vector<int> prev_uncover(n + 1, g - 1);
        vector<double> sum(n + 1);
        vector<int> query_cover(g, 0);
        
        query_cover[0] = 1;
        int current_cover = 1;

        edgetype now_rig_mx = min_full_cover, now_dijk_mx = 0;
        int now_dis_allowed_cur = start_cur;
        
        auto updata_dis_allowed_mx = [&]()
        {
            while(now_dis_allowed_cur + 1 < (int)cost_list.size() && cost_list[now_dis_allowed_cur + 1].first <= now_rig_mx)
                now_dis_allowed_cur++;
            for(int i = 1; i <= g - 1; i++)
                now_dijk_mx = max(now_dijk_mx, now_rig_mx*i - dis_allowed_mx[now_dis_allowed_cur][i]);
        };
        updata_dis_allowed_mx();

        pair<edgetype, int> chs = {INF, -1};
        auto find_dis_allowed_mx = [&]()
        {
            now_rig_mx = chs.first;
            int it = lower_bound(cost_list.begin() + 1, cost_list.end(), now_rig_mx, [&](auto x, auto y){return x.first < y;}) - cost_list.begin() - 1;
            now_dijk_mx = 0;
            it = max(1, it);
            for(int i = 1; i <= g - 1; i++)
                now_dijk_mx = max(now_dijk_mx, now_rig_mx*i - dis_allowed_mx[it][i]);
        };

        int fisrt_add = 1;
        auto add_point = [&](int u)
        {
            useful_points.push_back(u);
            dijk_rk[u] = useful_points.size();

            sum[u] = dis[u];
            prev_uncover[u] = 0;
            best[u] = 0;

            int n_prev_uncover = 0;
            edgetype n_sum = 0;
            for(int j = 1; j <= g - 1; j++)
            {
                int qid = LS[u][j];
                if(!query_cover[qid])
                {
                    n_sum = sum[u] + dist[qid][u];
                    n_prev_uncover = prev_uncover[u] + 1;

                    if(n_prev_uncover == 1 || n_sum / n_prev_uncover < sum[u] / prev_uncover[u])
                    {
                        best[u] = j;
                        sum[u] = n_sum;
                        prev_uncover[u] = n_prev_uncover;
                    }
                    else
                        break;
                }
            }

            if(fisrt_add) return;

            if(sum[u] / prev_uncover[u] < chs.first)
            {
                chs = {sum[u] / prev_uncover[u], u};
                find_dis_allowed_mx();
            }
        };

        auto run_dijkstra = [&]()
        {
            while(!pq.empty())
            {
                if(pq.top().first > now_dijk_mx)
                    return;
                auto [d, u] = pq.top();
                pq.pop();
                if(d > dis[u]) continue;

                add_point(u);

                for(auto [v, w] : graph.get_adj()[u])
                {
                    if(dis[v] > dis[u] + w)
                        dis[v] = dis[u] + w, pre[v] = u, pq.push({dis[v], v});
                }
            }
        };        
        run_dijkstra();
        fisrt_add = 0;

        average_point_ratio_0 += useful_points.size() * 1.0 / n;
        
        bool fine = 1;

        edgetype now_sum_weight = 0;

        int mx_dijk_rk = 0;

        while(current_cover != g) 
        {
            chs = {INF, -1};
            for(auto u : useful_points)
            {
                if(sum[u] / prev_uncover[u] < chs.first)
                    chs = {sum[u] / prev_uncover[u], u};
            }
            find_dis_allowed_mx();

            run_dijkstra();

            if(now_sum_weight + chs.first * (g - current_cover) > now_min_ans)
            {
                fine = 0;
                break;
            }

            now_sum_weight += chs.first;
            int c = chs.second;
            mx_dijk_rk = max(mx_dijk_rk, dijk_rk[c]);
            if(!add_edge(c, pre, dis, now_answer))
            {
                fine = 0;
                break;
            }

            int cur_best_c = best[c];
            for(int i = 1; i <= cur_best_c; i++) if(!query_cover[LS[c][i]])
            {
                current_cover++;
                int qid = LS[c][i];
                query_cover[qid] = 1;
                if(!add_edge(c, prev[qid], dist[qid], now_answer)) 
                {
                    fine = 0;
                    break;
                }
                for(auto j : useful_points) 
                {
                    if(rk[j][qid] <= best[j]) 
                    {
                        sum[j] -= dist[qid][j];
                        prev_uncover[j] -= 1;

                        while(best[j] != g - 1) 
                        {
                            best[j]++;
                            int n_qid = LS[j][best[j]];
                            if(query_cover[n_qid]) continue;

                            if(prev_uncover[j] == 0 || (sum[j] + dist[n_qid][j]) / (prev_uncover[j] + 1) < sum[j] / prev_uncover[j])
                            {
                                sum[j] += dist[n_qid][j];
                                prev_uncover[j] += 1;
                            }
                            else 
                                --best[j];
                            break;
                        }
                    }
                }
            }
            if(fine == 0) break;
        }
        average_point_ratio_all += useful_points.size() * 1.0 / n;
        dijk_rk_sum += mx_dijk_rk * 1.0 / n;
        dijk_rk_cnt++;

        if(!fine) continue;
        
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
        

        if(current_cover == g && now_answer.get_sum_weight() < now_min_ans) 
        {
            now_min_ans = now_answer.get_sum_weight();
            answer = now_answer;
        }
    }
    Timer::stop("MAIN", LogLevel::LOG_INFO, 0);

    if(now_min_ans > min_full_cover*(g - 1))
    {
        Tree<edgetype> now_answer;
        int c = min_full_cover_v;
        for(int i = 0; i < g; i++)
            add_edge(c, prev[LS[c][i]], dist[LS[c][i]], now_answer);
        if(now_answer.get_sum_weight() < now_min_ans)
            answer = now_answer;
    }


    Log::debug("n: " + to_string(n) + " g: " + to_string(g));
    Log::debug("query[0].size(): " + to_string(query[0].size()));
    // Log::info("average_point_ratio_0: " + to_string(average_point_ratio_0 / query[0].size()));
    // Log::info("average_point_ratio_all: " + to_string(average_point_ratio_all / query[0].size()));
    // Log::info("dijk_rk_sum: " + to_string(dijk_rk_sum / dijk_rk_cnt));

    return answer;
} 
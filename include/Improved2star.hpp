#pragma once
#include "Graph.hpp"
#include "GlobalUtils.hpp"
#include "BinaryHeap.hpp"
#include "StaticHeap.hpp"
#include "Log.hpp"
#include <vector>
using namespace std;

template<typename edgetype>
class Improved2star
{
private:
    edgetype INF;
    int n, g;
    Tree <edgetype> answer;
    edgetype now_min_ans;
    vector <vector <edgetype> > dist;
    vector <vector <int> > prev;
    vector <vector<int>> LS;
    vector <vector<int>> rk;
    vector <vector <pair <edgetype, int> > > cost_list_1;

    vector <pair<edgetype, int> > cost_list;
    vector <vector <edgetype> > dis_allowed_mx;
    vector <edgetype> dis_allowed_mx_1;
    vector <vector <array<edgetype, 3> > > dis_allowed_mx_2;
    vector <vector <int> > idx;
    vector <pair<int, int> > f_idx;
    vector <int> used_idx;

    vector <vector<pair<int, edgetype>> > ed;
    vector <int> deg;
    vector <vector <int> > cover_groups;

    edgetype start_dijk;

    edgetype min_full_cover;
    int min_full_cover_v;

    double average_point_ratio_0, average_point_ratio_all;
    vector<int> dijk_rk;
    double dijk_rk_sum;
    int dijk_rk_cnt;
    vector <int> cover_size_cnt;
    vector <double> cover_size_dijk;
    void init_debug_info()
    {
        average_point_ratio_0 = 0, average_point_ratio_all = 0;
        dijk_rk = vector<int> (n + 1, 0);
        dijk_rk_sum = 0;
        dijk_rk_cnt = 0;
        cover_size_cnt = vector <int> (g + 1, 0);
        cover_size_dijk = vector <double> (g + 1, 0);
    }


    vector <edgetype> dis;
    vector <int> pre;
    priority_queue <pair<edgetype, int>, vector<pair<edgetype, int>>, greater<pair<edgetype, int>>> pq;
    vector <int> useful_points;
    vector <int> best;
    vector <int> prev_uncover;
    vector <double> sum;
    vector <int> query_cover;
    int res_uncover, res_uncover_2;
    edgetype now_dijk_mx;
    pair<edgetype, int> chs;
    int current_cover;
    int first_add;


    /* 预处理 query 集合顺序 和 trivial 情况提前返回 */
    bool init(const Graph<edgetype>& graph, vector<vector<int>>& query)
    {
        answer = Tree<edgetype>(INF);
        n = graph.get_n(), g = query.size();

        if(g == 1) 
        {
            answer = Tree<edgetype>(0);
            return false;
        }

        for(int i = 1; i < g; i++)
            if(query[i].size() < query[0].size())
                swap(query[i], query[0]);
        
        if(query[0].size() == 0)
            return false;

        vector <int> cover_weight(n + 1, 0);
        for(int i = 0; i < g; i++)
        {
            for(auto x : query[i])
                cover_weight[x] ++;
        }
        sort(query[0].begin(), query[0].end(), [&](int i, int j){return cover_weight[i] > cover_weight[j];});
        
        if(cover_weight[query[0][0]] == g)
        {
            answer = Tree<edgetype>(0);
            return false;
        }

        ed = vector <vector<pair<int, edgetype>> > (n + 1);
        deg = vector <int> (n + 1, 0);
        cover_groups = vector <vector <int> > (n + 1);
        for(int i = 0; i < g; i++)
            for(auto x : query[i])
                cover_groups[x].push_back(i);
        
        return true;
    }

    /* 预处理询问组到每个点的最短路 */
    void init_dist(const Graph<edgetype>& graph, const vector<vector<int>>& query)
    {
        dist = vector <vector <edgetype> > (g, vector <edgetype> (n + 1, INF));
        prev = vector <vector <int> > (g, vector <int> (n + 1, -1));
        LS = vector<vector<int>> (n + 1, vector<int>(g));
        cost_list_1 = vector <vector <pair <edgetype, int> > > (g + 1, vector <pair <edgetype, int> >(1));

        for (auto& v : LS) iota(v.begin(), v.end(), 0);
        edgetype max_dis = 0, D = graph.get_min_weight();

        vector <vector <pair <edgetype, int> > > parti_queue;
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


        for(int ki = 0; ki < g; ki++)
        {
            vector<int> extended(n + 1);

            for(auto x : query[ki])
                dist[ki][x] = 0, enqueue(x, 0);
            while(!parti_empty())
            {
                auto [d, u] = dequeue();
                if(extended[u]) continue;
                cost_list_1[ki].push_back({d, u});
                extended[u] = 1;
                max_dis = max(max_dis, d);
                for(auto [v, w] : graph.get_adj()[u])
                {
                    if(dist[ki][v] / D > (dist[ki][u] + w) / D) 
                        enqueue(v, w + dist[ki][u]);

                    if(dist[ki][v] > dist[ki][u] + w)
                        dist[ki][v] = dist[ki][u] + w, prev[ki][v] = u;
                }
            }   
            // Log::info("parti_queue.size() : " + to_string(parti_queue.size()));
            reset_queue();
        }


        rk = vector<vector<int>> (n + 1, vector<int>(g));
        for(int c = 1; c <= n; c++)
        {
            sort(LS[c].begin() + 1, LS[c].end(), [&](int i, int j){return dist[i][c] < dist[j][c];});
            for(int j = 1; j <= g - 1; j++) {
                rk[c][LS[c][j]] = j;
            }
        }
    }
    
    /* 计算简单估值和对应中心 */
    void calc_min_full_cover()
    {
        min_full_cover = INF;
        min_full_cover_v = 0;
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

        Tree<edgetype> now_answer;
        int c = min_full_cover_v;
        for(int i = 0; i < g; i++)
            add_edge(c, prev[LS[c][i]], dist[LS[c][i]], now_answer);
        answer = now_answer;
        now_min_ans = answer.get_sum_weight();
    }

    /* 预处理剩 2 个集合时的估值 */
    void init_build_list_2()
    {
        cost_list = vector <pair <edgetype, int> > (1);
        dis_allowed_mx = vector <vector <edgetype> > (g + 1);
        dis_allowed_mx_1 = vector <edgetype> (g + 1, INF);
        dis_allowed_mx_2 = vector <vector <array<edgetype, 3> > > (g*(g + 1)/2 + 1);
        idx = vector <vector <int> > (g + 1, vector <int>(g + 1, 0));
        f_idx = vector <pair<int, int> > (g*(g + 1)/2 + 1, {0, 0});
        
        int idx_cnt = 0;
        for(int i = 1; i <= g; i++) for(int j = i + 1; j <= g; j++)
            idx[j][i] = idx[i][j] = ++idx_cnt, f_idx[idx_cnt] = {i, j};
        used_idx = vector <int> (idx_cnt + 1, 0);
    }
    
    /* 在线建立状态 i 对应的表 */
    void build_list_2(int i)
    {
        auto [u, v] = f_idx[i];
        int p1 = 1, p2 = 1;
        int len1 = cost_list_1[u].size(), len2 = cost_list_1[v].size();
        auto Merge_sort_next = [&]()
        {
            if(p1 < len1 && p2 < len2)
            {
                if(cost_list_1[u][p1].first < cost_list_1[v][p2].first)
                    return cost_list_1[u][p1++];
                else
                    return cost_list_1[v][p2++];
            }
            if(p1 < len1) return cost_list_1[u][p1++];
            if(p2 < len2) return cost_list_1[v][p2++];
            assert(0);
        };
        
        int len = len1 + len2 - 2;
        vector <pair<edgetype, int> > pre_info(n + 1);
        pair <edgetype, edgetype> mi_2 = {INF, INF};

        for(int j = 1; j <= len; j++)
        {
            auto [cost, u] = Merge_sort_next();
            pre_info[u].first += cost;
            pre_info[u].second++;
            int updated = 0;
            if(pre_info[u].second == 1) 
            {
                if(mi_2.first > pre_info[u].first)
                {
                    mi_2.first = pre_info[u].first;
                    updated = 1;
                }
            }
            else
            {
                if(mi_2.second > pre_info[u].first)
                {
                    mi_2.second = pre_info[u].first;
                    updated = 1;
                }
            }
            if(updated)
                dis_allowed_mx_2[i].push_back({cost, mi_2.first, mi_2.second});
        }
    }

    /* 计算全部集合时的估值 */
    void calc_dis_allowed_mx()
    {
        for(int i = 1; i <= n; i++) 
            for(int j = 1; j <= g - 1; j++)
            {
                if(dist[LS[i][j]][i] > min_full_cover*(g - 1))
                    break;
                dis_allowed_mx_1[LS[i][j]] = min(dis_allowed_mx_1[LS[i][j]], dist[LS[i][j]][i]);
            }

        for(int i = 1; i <= g - 1; i++)
            while(!cost_list_1[i].empty() && cost_list_1[i].back().first > min_full_cover*(g - 1))
                cost_list_1[i].pop_back();

        auto Build_cost_list = [&]()
        {
            vector <int> pos(g + 1, 1);
            auto cmp = [&](const pair<int, int>& a, const pair<int, int>& b) {
                return cost_list_1[a.second][a.first].first > cost_list_1[b.second][b.first].first;
            };
            priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(cmp)> pq(cmp);
            auto ins = [&](int i)
            {
                if(pos[i] < (int)cost_list_1[i].size())
                    pq.push({pos[i]++, i});
            };
            for(int i = 1; i <= g - 1; i++)
                ins(i);
            while(!pq.empty())
            {
                auto [p, i] = pq.top();
                pq.pop();
                cost_list.push_back(cost_list_1[i][p]);
                ins(i);
            }
        };
        // Timer::start("Build_cost_list");
        Build_cost_list();
        // Timer::stop("Build_cost_list", LogLevel::LOG_INFO);

        // Timer::start("aft");
        
        int len = cost_list.size();

        array<edgetype, 10> now_mi;
        vector <pair<edgetype, int> > pre_info(n + 1);

        for(int i = 0; i <= g - 1; i++) now_mi[i] = INF;


        for(int i = 1; i < len; i++)
        {
            auto [cost, u] = cost_list[i];
            pre_info[u].first += cost;
            pre_info[u].second++;
            if(now_mi[pre_info[u].second] > pre_info[u].first) 
            {
                now_mi[pre_info[u].second] = pre_info[u].first;
                dis_allowed_mx[0].push_back(cost);
                for(int j = 1; j <= g - 1; j++)
                    dis_allowed_mx[j].push_back(now_mi[j]);
                if(cost < min_full_cover)
                {
                    start_dijk = 0;
                    for(int j = 1; j <= g - 1; j++)
                        start_dijk = max(start_dijk, min_full_cover*j - dis_allowed_mx[j].back());
                }    
            }
        }
    }

    /* 新选根时的预处理 */
    void init_now_root()
    {
        dis = vector <edgetype> (n + 1, INF);
        pre = vector <int> (n + 1, -1);
        while(!pq.empty())
            pq.pop();
        useful_points = vector <int> ();
        best = vector <int> (n + 1, g - 1);
        prev_uncover = vector <int> (n + 1, g - 1);
        sum = vector <double> (n + 1, 0);
        query_cover = vector <int> (g, 0);
        query_cover[0] = 1;
        res_uncover = 0, res_uncover_2 = 0;
        now_dijk_mx = start_dijk;
        chs = {INF, -1};
    
        first_add = 1;
    }

    /* 更新当前状态的 dijk 最大值 */
    void find_dis_allowed_mx()
    {
        edgetype now_rig_mx = chs.first;
        if(current_cover == g - 1)
        {
            now_dijk_mx = now_rig_mx - dis_allowed_mx_1[res_uncover];
            return;
        }
        else if(current_cover == g - 2)
        {
            if(!used_idx[res_uncover_2])
            {
                used_idx[res_uncover_2] = 1;
                build_list_2(res_uncover_2);
            }
            int it = lower_bound(dis_allowed_mx_2[res_uncover_2].begin(), dis_allowed_mx_2[res_uncover_2].end(), now_rig_mx, 
            [&](auto x, auto y){return x[0] < y;}) - dis_allowed_mx_2[res_uncover_2].begin() - 1;
            it = max(0, it);
            now_dijk_mx = max(now_rig_mx - dis_allowed_mx_2[res_uncover_2][it][1], now_rig_mx*2 - dis_allowed_mx_2[res_uncover_2][it][2]);
            return;
        }

        int it = lower_bound(dis_allowed_mx[0].begin(), dis_allowed_mx[0].end(), now_rig_mx, [&](auto x, auto y){return x < y;}) - dis_allowed_mx[0].begin() - 1;
        now_dijk_mx = 0;
        it = max(0, it);
        for(int i = 1; i <= g - 1 - current_cover; i++)
            now_dijk_mx = max(now_dijk_mx, now_rig_mx*i - dis_allowed_mx[i][it]);
    }

    /* 给 now_answer 添加一条边 */
    bool add_edge(int u, vector <int> &par, vector <edgetype> &weight, Tree<edgetype> &now_answer)
    {
        while(par[u] != -1)
        {
            now_answer.add_edge(u, par[u], weight[u] - weight[par[u]]);
            if(now_answer.get_sum_weight() >= answer.get_sum_weight()) 
                return false;
            u = par[u];
        }   
        return true; 
    }

    /* dijk 新确定一个节点 dis(r,u) */
    void add_point(int u)
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

        if(first_add) return;

        if(sum[u] / prev_uncover[u] < chs.first)
        {
            chs = {sum[u] / prev_uncover[u], u};
            find_dis_allowed_mx();
        }
    }
    /* 继续跑一段 dijk */
    void run_dijkstra(const Graph<edgetype>& graph)
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
    }

    /* 更新当前状态的 res_uncover */
    void updata_res_uncover()
    {
        if(current_cover == g - 1)
        {
            for(int i = 1; i <= g - 1; i++) if(!query_cover[i])
                res_uncover = i;
        }
        else if(current_cover == g - 2)
        {
            vector <int> tmp_uncover;
            for(int i = 1; i <= g - 1; i++) if(!query_cover[i])
                tmp_uncover.push_back(i);
            res_uncover_2 = idx[tmp_uncover[0]][tmp_uncover[1]];
        }
    }

    /* 重建答案 */
    void Rebuild_Tree(Tree<edgetype> &now_answer)
    {
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


        auto ck_no_mid_point = [&]()
        {
            vector <int> cover_by_v(g, 0);
            set <int> cover_v;
            int covered_cnt = 0;
            for(auto x : useful_points)
            {
                for(auto y: cover_groups[x])
                    if(cover_by_v[y] == 0)
                        cover_by_v[y] = x, cover_v.insert(x), ++covered_cnt;
            }
            if(covered_cnt != g) return;
            Tree<edgetype> now_ans;
            for(auto x : cover_v)
                add_edge(x, pre, dis, now_ans);
            if(now_ans.get_sum_weight() < now_min_ans)
                now_min_ans = now_ans.get_sum_weight(), answer = now_ans;// system("pause");
        };
        ck_no_mid_point();
    }

public:
    Improved2star()
    {
        INF = numeric_limits<edgetype>::max();
        answer = Tree<edgetype>(INF);
    }
    
    Tree<edgetype> solve(const Graph<edgetype>& graph, vector<vector<int>>& query)
    {
        if(!init(graph, query))
            return answer;
        
        init_dist(graph, query);

        calc_min_full_cover();
        
        init_build_list_2();

        calc_dis_allowed_mx();
        //Log::info("trivial answer " + to_string(answer.get_sum_weight()));

        init_debug_info();

        for(auto r: query[0])
        {
            Tree<edgetype> now_answer;

            init_now_root();
            
            /* 先对 min_full_cover 跑一段 dijk */
            current_cover = 1;
            dis[r] = 0;
            pq.push({0, r}); 
            run_dijkstra(graph);
            first_add = 0;

            int mx_dijk_rk = 0;

            bool fine = 1;
            edgetype now_sum_weight = 0;

            while(current_cover != g) 
            {
                updata_res_uncover();

                chs = {INF, -1};
                for(auto u : useful_points)
                {
                    if(sum[u] / prev_uncover[u] < chs.first)
                        chs = {sum[u] / prev_uncover[u], u};
                }
                find_dis_allowed_mx();
                run_dijkstra(graph);

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
                                {
                                    --best[j];
                                    break;
                                }    
                            }
                        }
                    }
                }

                if(fine == 0) break;
                cover_size_cnt[current_cover]++;
                cover_size_dijk[current_cover] += useful_points.size() * 1.0 / n;
            }


            average_point_ratio_all += useful_points.size() * 1.0 / n;
            dijk_rk_sum += mx_dijk_rk * 1.0 / n;
            dijk_rk_cnt++;

            if(!fine) continue;

            Rebuild_Tree(now_answer);
        }
        
        return answer;
    }
};
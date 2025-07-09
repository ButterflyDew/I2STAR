#include "../include/Graph.hpp"
#include "../include/Log.hpp"
#include "../include/GlobalUtils.hpp"
#include "../include/GSTCover_v2.hpp"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <graphname>\nExample: " << argv[0] << " Toronto" << endl;
        return 1;
    }
    Log::setLogFile("log_" + string(argv[1]) + "_v6.txt");
    Log::setConsoleLevel(LogLevel::LOG_INFO);   
    Log::setFileLevel(LogLevel::LOG_IMPORTANT);   

    string graphname = argv[1];
    string dirname = "data/" + graphname + "/";
    fs_filesystem = dirname;
    Graph<double> g;
    auto query = read_query_file();
    Log::debug("Loaded everything");
    for(int i = 0; i < (int)query.size(); i++)
    {
        int gsize = query[i].size();
        map <int, vector <vector<int>>> query_new;
        for(int j = 0; j < gsize; j++)
        {
            for(auto x: query[i][j])
            {
                int rt = g.Find(x);
                if(query_new.find(rt) == query_new.end())
                    query_new[rt] = vector <vector<int>>(gsize, vector<int>());
                query_new[rt][j].push_back(x);
            }
        }

        Log::info("Query " + to_string(i) + ": ");
        Timer::start("Query " + to_string(i));

        Tree<double> t(numeric_limits<double>::max());
        for(auto [_, qn]: query_new)
        {
            auto tn = gst_cover_v2(g, qn);
            if(tn.get_sum_weight() < t.get_sum_weight())
                t = tn;
        }


        auto duration = Timer::stop("Query " + to_string(i), LogLevel::LOG_INFO, false);
        double sum_weight = t.get_sum_weight();
        if(sum_weight == numeric_limits<double>::max()) sum_weight = -1;
        Log::log(LogLevel::LOG_INFO, "Time: " + to_string(duration) + "s" + " sum_weight: " + to_string(sum_weight));
        Log::log(LogLevel::LOG_IMPORTANT, to_string(duration) + " " + to_string(sum_weight));

        //system("pause");
    }
    
    return 0;
} 
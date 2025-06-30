#include "../include/Graph.hpp"
#include "../include/GlobalUtils.hpp"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <graphname>\nExample: " << argv[0] << " LinkedMDB" << endl;
        return 1;
    }
    string graphname = argv[1];
    string dirname = "data/" + graphname + "/";
    fs_filesystem = dirname;

    try {
        // Assume edge type is double
        Graph<double> g;
        cout << "Graph loaded. Nodes: " << g.get_n() << ", Edges: " << g.get_m() << endl;
        // Print outgoing edges for first 5 nodes
        const auto& adj = g.get_adj();
        for (int u = 1; u <= min(g.get_n(), 5); ++u) {
            cout << "Node " << u << " outgoing: ";
            for (auto [v, w] : adj[u]) {
                cout << "(" << v << ", " << w << ") ";
            }
            cout << endl;
        }
        // Print edge_map
        cout << "Sample edge_map: ";
        int cnt = 0;
        for (const auto& [p, w] : g.get_edge_map()) {
            cout << "(" << p.first << ", " << p.second << "): " << w << "; ";
            if (++cnt >= 5) break;
        }
        cout << endl;
    } catch (const exception& e) {
        cout << "Graph load failed: " << e.what() << endl;
        return 1;
    }

    try {
        auto all_queries = read_query_file();
        cout << "Total " << all_queries.size() << " queries." << endl;
        for (int i = 0; i < min((int)all_queries.size(), 3); ++i) {
            cout << "Query group " << i+1 << ": ";
            for (const auto& group : all_queries[i]) {
                cout << "{";
                for (int v : group) cout << v << ' ';
                cout << "} ";
            }
            cout << endl;
        }
    } catch (const exception& e) {
        cout << "Query load failed: " << e.what() << endl;
        return 1;
    }
    return 0;
} 
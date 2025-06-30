#include "../include/BinaryHeap.hpp"
#include "../include/StaticHeap.hpp"
#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#include <algorithm>
using namespace std;

struct Node {
    int id;
    int value;
    bool operator<(const Node& other) const {
        return value < other.value;
    }
};

int main() {
    cout << "==== BinaryHeap (STL-like) test ====" << endl;
    BinaryHeap<Node> bh(10);
    bh.push({1, 50});
    bh.push({2, 30});
    bh.push({3, 80});
    bh.push({4, 10});
    assert(bh.top().value == 10);
    bh.pop();
    assert(bh.top().value == 30);
    bh.pop();
    assert(bh.top().value == 50);
    cout << "BinaryHeap test passed!" << endl;

    cout << "==== StaticHeap (static, modifiable) test ====" << endl;
    vector<Node> data = {{}, {0, 5}, {1, 3}, {2, 8}, {3, 1}, {4, 7}}; // data[0]无效
    StaticHeap<Node> sh(data);
    assert(sh.top().value == 1);
    // 修改 data[3] 的值为0，调整堆
    sh.modify(3, {2, 0});
    assert(sh.top().id == 2 && sh.top().value == 0);
    sh.pop();
    assert(sh.top().value == 1);
    cout << "StaticHeap test passed!" << endl;

    // 大数据暴力验证
    cout << "==== Randomized StaticHeap brute-force check ====" << endl;
    int N = 1000;
    vector<Node> big_data(N + 1);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, 1000000);
    for (int i = 1; i <= N; ++i) {
        big_data[i] = Node{i, dist(rng)};
    }
    StaticHeap<Node> big_sh(big_data);
    vector<Node> brute(big_data);
    vector<bool> valid(N + 1, true); // 是否还在堆中
    for (int op = 0; op < 500; ++op) {
        int type = dist(rng) % 3;
        if (type == 0) { // 修改某个元素
            int idx = 1 + dist(rng) % N;
            if (!valid[idx]) continue;
            int new_val = dist(rng);
            big_sh.modify(idx, {idx, new_val});
            brute[idx].value = new_val;
        } else if (type == 1) { // 弹出最小值
            if (big_sh.empty()) continue;
            Node heap_top = big_sh.top();
            big_sh.pop();
            int min_idx = -1;
            for (int i = 1; i <= N; ++i) {
                if (valid[i]) {
                    if (min_idx == -1 || brute[i].value < brute[min_idx].value) min_idx = i;
                }
            }
            assert(min_idx != -1);
            assert(brute[min_idx].value == heap_top.value && brute[min_idx].id == heap_top.id);
            valid[min_idx] = false;
        } else { // 查询最小值
            if (big_sh.empty()) continue;
            Node heap_top = big_sh.top();
            int min_idx = -1;
            for (int i = 1; i <= N; ++i) {
                if (valid[i]) {
                    if (min_idx == -1 || brute[i].value < brute[min_idx].value) min_idx = i;
                }
            }
            assert(min_idx != -1);
            assert(brute[min_idx].value == heap_top.value && brute[min_idx].id == heap_top.id);
        }
    }
    cout << "Randomized StaticHeap test passed!" << endl;
    cout << "All heap tests passed!" << endl;
    return 0;
} 
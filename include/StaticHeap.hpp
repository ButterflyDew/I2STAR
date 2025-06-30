#pragma once
#include <vector>
#include <stdexcept>
using namespace std;

template<typename T>
class StaticHeap {
public:
    // data[0] 无效，data[1..n] 有效
    StaticHeap(const std::vector<T>& data) : cap((int)data.size() - 1) {
        heap.reserve(cap + 2);
        heap.push_back({T(), -1}); // 1-indexed 占位
        pos.assign(data.size(), -1); // pos[0]无效
        for (int i = 1; i < (int)data.size(); ++i) {
            heap.push_back({data[i], i});
            pos[i] = i;
        }
        for (int i = cap / 2; i >= 1; --i) down(i);
        for (int i = 1; i <= cap; ++i) {
            int data_idx = heap[i].second;
            pos[data_idx] = i;
        }
    }
    void pop() {
        if (empty()) throw underflow_error("Heap empty");
        int data_idx = heap[1].second;
        if (data_idx >= 1 && data_idx < (int)pos.size()) pos[data_idx] = -1;
        heap[1] = heap.back();
        int new_idx = heap[1].second;
        if (new_idx >= 1 && new_idx < (int)pos.size()) pos[new_idx] = 1;
        heap.pop_back();
        if (!empty()) down(1);
    }
    T top() const {
        if (empty()) throw underflow_error("Heap empty");
        return heap[1].first;
    }
    bool empty() const { return heap.size() <= 1; }
    int size() const { return (int)heap.size() - 1; }
    void modify(int data_idx, const T& new_val) {
        int idx = pos[data_idx];
        if (idx == -1) return;
        heap[idx].first = new_val;
        up(idx);
        down(idx);
    }
private:
    vector<pair<T, int>> heap; // 1-indexed, second为data下标
    vector<int> pos; // pos[i]为data[i]在堆中的下标
    int cap;
    void up(int idx) {
        while (idx > 1 && heap[idx].first < heap[idx / 2].first) {
            pos[heap[idx].second] = idx / 2;
            pos[heap[idx / 2].second] = idx;
            swap(heap[idx], heap[idx / 2]);
            idx /= 2;
        }
    }
    void down(int idx) {
        int n = (int)heap.size() - 1;
        while (2 * idx <= n) {
            int j = 2 * idx;
            if (j + 1 <= n && heap[j + 1].first < heap[j].first) ++j;
            if (!(heap[j].first < heap[idx].first)) break;
            pos[heap[idx].second] = j;
            pos[heap[j].second] = idx;
            swap(heap[idx], heap[j]);
            idx = j;
        }
    }
}; 
#pragma once
#include <vector>
#include <stdexcept>
using namespace std;

// 手写二叉堆模板类，支持自定义 < 的类型

template<typename T>
class BinaryHeap {
public:
    BinaryHeap(int capacity) : cap(capacity) {
        //heap.reserve(capacity + 1);
        heap.push_back(T()); // 1-indexed 占位
    }
    void push(const T& val) {
        //if ((int)heap.size() > cap) throw overflow_error("Heap full");
        heap.push_back(val);
        up((int)heap.size() - 1);
    }
    void pop() {
        if (empty()) throw underflow_error("Heap empty");
        heap[1] = heap.back();
        heap.pop_back();
        if (!empty()) down(1);
    }
    T top() const {
        if (empty()) throw underflow_error("Heap empty");
        return heap[1];
    }
    bool empty() const { return heap.size() <= 1; }
    int size() const { return (int)heap.size() - 1; }
private:
    vector<T> heap;
    int cap;
    void up(int idx) {
        while (idx > 1 && heap[idx] < heap[idx / 2]) {
            swap(heap[idx], heap[idx / 2]);
            idx /= 2;
        }
    }
    void down(int idx) {
        int n = (int)heap.size() - 1;
        while (2 * idx <= n) {
            int j = 2 * idx;
            if (j + 1 <= n && heap[j + 1] < heap[j]) ++j;
            if (!(heap[j] < heap[idx])) break;
            swap(heap[idx], heap[j]);
            idx = j;
        }
    }
}; 
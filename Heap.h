#ifndef HEAP_H_
#define HEAP_H_

#include <vector>
#include <cmath>

template <class K, class T>
class Heap {
    public:
        void insert(K key, T item);

        T extractMin();

        void heapify();

        bool empty();
    private:
        int parent(int index);

        int leftChild(int index);

        int rightChild(int index);

        void swap(int left, int right);

    private:
        std::vector<std::pair<K, T>> heap;
};

template <class K, class T>
void Heap<K, T>::insert(K key, T item) { 
    this->heap.push_back({key, item});
    auto itemIndex = this->heap.size() - 1;

    if (itemIndex > 0) {
        while (heap[itemIndex].first < heap[parent(itemIndex)].first) {
            this->swap(itemIndex, parent(itemIndex));
            itemIndex = parent(itemIndex);
        }
    }
}

template <class K, class T>
T Heap<K, T>::extractMin() {
    auto minimum = this->heap[0].second;
    this->heap[0] = this->heap.pop_back();

    auto outerIndex = 0;
    auto innerIndex = 0;
    auto isHeap = false;

    while (!isHeap && outerIndex * 2 < this->heap.size()) {
        innerIndex = leftChild(outerIndex);

        if (innerIndex < this->heap.size() - 1) {
            if (this->heap[innerIndex].first > this->heap[innerIndex + 1].first) {
                innerIndex += 1;
            }
        }

        // not a min heap
        if (this->heap[innerIndex].first < this->heap[outerIndex].first) {
            this->swap(innerIndex, outerIndex);
            outerIndex = innerIndex;
        }
        else {
            isHeap = true;
        }
    }

    return minimum;
}

template <class K, class T>
void Heap<K, T>::heapify() {
    for (auto index = std::floor(this->heap.size() / 2); index >= 0; index -= 1) {
        auto outerIndex = index;
        auto innerIndex = index;
        auto isHeap = false;

        while (!isHeap && outerIndex * 2 < this->heap.size()) {
            innerIndex = leftChild(outerIndex);

            if (innerIndex < this->heap.size() - 1) {
                if (this->heap[innerIndex].first > this->heap[innerIndex + 1].first) {
                    innerIndex += 1;
                }
            }

            // not a min heap
            if (this->heap[innerIndex].first < this->heap[outerIndex].first) {
                this->swap(innerIndex, outerIndex);
                outerIndex = innerIndex;
            }
            else {
                isHeap = true;
            }
        }
    }
}

template <class K, class T>
bool Heap<K, T>::empty() {
    return this->heap.size() == 0;
}

template <class K, class T>
int Heap<K, T>::parent(int index) {
    return std::floor(((index + 1) / 2) - 1);
}

template <class K, class T>
int Heap<K, T>::leftChild(int index) {
    return ((index + 1) * 2) - 1;
}

template <class K, class T>
int Heap<K, T>::rightChild(int index) {
    return (index + 1) * 2;
}

template <class K, class T>
void Heap<K, T>::swap(int left, int right) {
    auto temp = this->heap[left];
    this->heap[left] = this->heap[right];
    this->heap[right] = temp;
}

#endif // Heap.h included

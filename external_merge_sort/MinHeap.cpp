//
// Created by giacomo on 16/04/19.
//

#include "MinHeap.h"

// Prototype of a utility function to swap two min heap nodes
// A utility function to swap two elements
void swap(MinHeapNode* x, MinHeapNode* y) {
    MinHeapNode temp = *x;
    *x = *y;
    *y = temp;
}

void MinHeap::replaceMin(MinHeapNode x) {
    harr[0] = x;
    MinHeapify(0);
}

MinHeap::MinHeap(std::vector<MinHeapNode>& a, int size) : harr{a} {
    heap_size = size;
    int i = (heap_size - 1) / 2;
    while (i >= 0)
    {
        MinHeapify(i);
        i--;
    }
}

void MinHeap::MinHeapify(int i) {
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if (l < heap_size && harr[l].element < harr[i].element)
        smallest = l;
    if (r < heap_size && harr[r].element < harr[smallest].element)
        smallest = r;
    if (smallest != i)
    {
        swap(&harr[i], &harr[smallest]);
        MinHeapify(smallest);
    }
}

int MinHeap::left(int i) { return (2 * i + 1); }

int MinHeap::right(int i) { return (2 * i + 2); }

MinHeapNode MinHeap::getMin() {  return harr[0]; }


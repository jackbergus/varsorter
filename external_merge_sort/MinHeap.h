//
// Created by giacomo on 16/04/19.
//

#ifndef VARSORTER_MINHEAP_H
#define VARSORTER_MINHEAP_H

#include <vector>
#include "MinHeapNode.h"

// A class for Min Heap
class MinHeap
{
    std::vector<MinHeapNode>& harr; // pointer to array of elements in heap
    int heap_size;     // size of min heap

public:
    // Constructor: creates a min heap of given size

// Constructor: Builds a heap from a given array a[]
// of given size
    MinHeap(std::vector<MinHeapNode>& a, int size);

    // to heapify a subtree with root at given index
    // A recursive method to heapify a subtree with root
// at given index. This method assumes that the
// subtrees are already heapified
    void MinHeapify(int);

    // to get index of left child of node at index i
    int left(int i);

    // to get index of right child of node at index i
    int right(int i);

    // to get the root
    MinHeapNode getMin();

    // to replace root with new node x and heapify()
    // new root
    void replaceMin(MinHeapNode x);
};

#endif //VARSORTER_MINHEAP_H

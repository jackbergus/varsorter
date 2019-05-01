//
// Created by giacomo on 16/04/19.
//

#include <vector>
#include "../src/inserter.h"
#include "../src/serializer_with_sort.h"
#include "MinHeapNode.h"
#include "MinHeap.h"

// Merges k sorted files.  Names of files are assumed
// to be 1, 2, 3, ... k
void mergeFiles(std::string &out_filename, std::string &out_index, int k, std::vector<serializer_with_sort> &in) {
    // FINAL OUTPUT FILE
    inserter out;
    out.open(out_index, out_filename);

    // Create a min heap with k heap nodes.  Every heap node
    // has first element of scratch output file
    std::vector<MinHeapNode> harr;
    harr.reserve(k);
    //MinHeapNode* harr = new MinHeapNode[k];
    int i;
    for (i = 0; i < k; i++){
        // break if no output file is empty and
        // index i will be no. of input files
        if (fscanf(in[i], "%d ", &harr[i].element) != 1)
            break;

        harr[i].i = i; // Index of scratch output file
    }
    MinHeap hp(harr, i); // Create the heap

    int count = 0;

    // Now one by one get the minimum element from min
    // heap and replace it with next element.
    // run till all filled input files reach EOF
    while (count != i)
    {
        // Get the minimum element and store it in output file
        MinHeapNode root = hp.getMin();
        fprintf(out, "%d ", root.element);

        // Find the next element that will replace current
        // root of heap. The next element belongs to same
        // input file as the current min element.
        if (fscanf(in[root.i], "%d ", &root.element) != 1 )
        {
            root.element = std::numeric_limits<int>::max();
            count++;
        }

        // Replace root with next element of input file
        hp.replaceMin(root);
    }

    // close input and output files
    for (int i = 0; i < k; i++)
        fclose(in[i]);

    fclose(out);
}
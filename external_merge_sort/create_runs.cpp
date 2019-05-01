//
// Created by giacomo on 16/04/19.
//

#include <vector>
#include <parallel/algorithm>
#include "../src/virtual_sorter.h"
#include "../src/smart_malloc.h"
#include "../src/inserter.h"
#include "../src/serializer_with_sort.h"

// Using a merge-sort algorithm, create the initial runs
// and divide them evenly among the output files
void createInitialRuns(std::string &input_file, std::string &input_index_file, int run_size, virtual_sorter &opener,
                       __compar_fn_t comparator, std::vector<serializer_with_sort>& out) {
    opener.openvirtual_sorter(input_index_file, input_file);

    // output scratch files;
    int num_ways = opener.data_serialized_file / run_size;
    out.reserve(num_ways);

    for (int i = 0; i < num_ways; i++)
    {
        // convert i to string
        std::string filename_value = std::to_string(i) + ".value";
        std::string filename_index = std::to_string(i) + ".idx";

        // Open output files in write mode.
        out.emplace_back(filename_index, filename_value);
    }

    // allocate a dynamic array large enough
    // to accommodate runs of size run_size
    std::vector<smart_malloc> arr;
    arr.reserve(run_size);
    //int* arr = (int*)malloc(run_size * sizeof(int));

    bool more_input = true;
    int next_output_file = 0;

    int i;
    virtual_sorter::iterator ptr = opener.begin(), fini = opener.end();
    while (more_input) {
        // write run_size elements into arr from input file
        for (i = 0; i < run_size; i++) {
            if (ptr == fini) {
                more_input = false;
                break;
            } else {
                if (!next_output_file) {
                    arr.emplace_back();
                }
                arr[i].docopy(*ptr);
                ptr++;
            }
        }

        // sort array using merge sort
        __gnu_parallel::sort(arr.begin(), arr.begin()+i, comparator);

        // write the records to the appropriate scratch output file
        // can't assume that the loop runs to run_size
        // since the last run's length may be less than run_size
        for (int j = 0; j < i; j++) {
            out[next_output_file].insert(arr[j].malloced_iovec);
        }
        out[next_output_file].close();
        next_output_file++;
    }

    // close input and output files
    opener.doclose();
}
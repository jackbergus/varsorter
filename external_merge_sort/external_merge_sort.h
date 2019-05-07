/*
 * external_merge_sort.h
 * This file is part of varsorter
 *
 * Copyright (C) 2019 - Giacomo Bergami
 *
 * varsorter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * varsorter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with varsorter. If not, see <http://www.gnu.org/licenses/>.
 */
//
// Created by giacomo on 16/04/19.
//

#ifndef VARSORTER_EXTERNAL_MERGE_SORT_H
#define VARSORTER_EXTERNAL_MERGE_SORT_H

#ifndef _GLIBCXX_PARALLEL
#define _GLIBCXX_PARALLEL
#endif

#include <string>
#include <vector>
#include <queue>
#include "../src/original/serializer_with_sort.h"
#include "../src/original/smart_malloc.h"
#include "quicksort.h"

/**
 * Data structure used by the min-Heap.
 * This structure is relevant not only because it stores each file value per time, but also because it stores which
 * file is associated to the given file
 */
struct miniheap_iovec {
    struct new_iovec iovec;
    int fileid;
    miniheap_iovec(void *memory, uint_fast64_t size, int fileno) : iovec{memory, size}, fileid{fileno} {}
};

/**
 * Unique class implementing the
 * @tparam QuicksortComparator      Specific implementation of the "QuicksortLessComparator" class. This is going to be
 *                                  used to both sort the data via the
 */
template <typename QuicksortComparator> class external_merge_sort {
    struct quicksort<QuicksortComparator> q;

    /**
     * Using a merge-sort algorithm, create the initial runs, and divide them evenly among the output files
     *
     * @param input_file            Input file containing all the values
     * @param input_index_file      Indexing file containing the pointer to the beginning and to the end of each serialized value
     * @param run_size              Maximum size allowed in primary memory
     * @param opener                Pointer to the file containing all the values
     * @param comparator            Comparator used to sort each chunk file in secondary memory
     * @param out                   (out) already sorted files, to be merged in the next step
     */
    void createInitialRuns(std::string &input_file, std::string &input_index_file, int run_size,
                           std::vector<serializer_with_sort>& out) {
        virtual_sorter opener;
        opener.openvirtual_sorter(input_index_file, input_file);

        // output scratch files;
        size_t num_ways = (opener.struct_index_size / (sizeof(struct index))) / run_size;
        size_t full = (opener.struct_index_size / (sizeof(struct index))), completed = 0;
        out.reserve(num_ways);

        for (int i = 0; i < num_ways; i++)
        {
            // convert i to string
            std::string filename_value = std::to_string(i) + ".value";
            std::string filename_index = std::to_string(i) + ".idx";

            // Open output files in write mode.
            out.emplace_back(filename_index, filename_value).sorter = new virtual_sorter();
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
        while (completed != full) {
            // write run_size elements into arr from input file
            int currentSize = 0;
            for (i = 0; i < run_size; i++) {
                if (ptr == fini || ptr->iov_base == nullptr) {
                    more_input = false;
                    break;
                } else {
                    if (!next_output_file) {
                        arr.emplace_back();
                    }
                    arr[i].docopy(*ptr);
                    completed++;
                    ptr++;
                    currentSize++;
                }
            }

            // TODO: in parallel
            q.do_quicksort(arr, currentSize);

            // write the records to the appropriate scratch output file
            // can't assume that the loop runs to run_size
            // since the last run's length may be less than run_size
            for (int j = 0; j < currentSize; j++) {
                out[next_output_file].insert(arr[j].malloced_iovec);
            }
            //out[next_output_file].close();
            next_output_file++;
        }

        // Forcing to free the memory
        for (smart_malloc& toReset : arr)
            toReset.moved = false;

        // close input and output files
        opener.doclose();
    }


/**
 * This other place sorts each file using a heap, which will store for each file the minimum. Heapsort is used to
 * minimize the memory occupation of each file to the minimum value of each file (which is each first location of every file).
 *
 * @param out_filename          File name where the values are going to be stored
 * @param out_index             Final index through which the values will be accessible
 * @param in                    Vector containing the pointer to all the files into which the initial file is chunked into
 */
   void mergeFiles(std::string &out_filename, std::string &out_index, std::vector<serializer_with_sort> &in) {
        // FINAL OUTPUT FILE
        inserter out;
        out.open(out_index, out_filename);

        std::vector<virtual_sorter::iterator> ptrs, ends;

        // Create a min heap with k heap nodes (miniheap_iovec).  Every heap node
        // has first element of scratch output file
        std::priority_queue<miniheap_iovec, std::vector<miniheap_iovec>, QuicksortComparator> minheap;


        int k = in.size();
        int i;
        for (i = 0; i < k; i++){
            virtual_sorter::iterator ptr = in[i].begin(), end = in[i].end();
            ptrs.push_back(ptr);
            ends.push_back(end);
            if (ptr == end)
                break;
            else {
                minheap.emplace(ptr->iov_base, ptr->iov_len, i);
            }
        }

        //int count = 0;
        long written = 0;

        // Now one by one get the minimum element from min
        // heap and replace it with next element.
        // run till all filled input files reach EOF
        while (!minheap.empty()) {
            // Get the minimum element and store it in output file
            struct miniheap_iovec x = minheap.top();
            int fileid = x.fileid;
            std::cout << std::string((const char*)x.iovec.iov_base, x.iovec.iov_len) << std::endl;
            out.writeKeyAndValue(x.iovec);
            minheap.pop();
            written++;
            ptrs[fileid]++;

            // Find the next element that will replace current
            // root of heap. The next element belongs to same
            // input file as the current min element.
            if ((ptrs[fileid] != ends[fileid])) {
                minheap.emplace(ptrs[fileid]->iov_base, ptrs[fileid]->iov_len, fileid);
            } else {
                delete in[fileid].sorter;
                in[fileid].sorter = nullptr;
                in[fileid].unlink();
            }
        }

        for (serializer_with_sort& ptr : in) {
            delete ptr.sorter;
            ptr.sorter = nullptr;
        }

        out.close();
    }


public:

    /**
     * Before running the method, ensure that these two files are closed
     * @param values
     * @param index
     * @param run_size
     */
    void run(std::string& values, std::string& index, size_t run_size) {
        std::vector<serializer_with_sort> vectore;
        createInitialRuns(values, index, run_size, vectore);
        std::string out_filename{"temp.txt"};
        std::string out_index{"temp.idx"};
        mergeFiles(out_filename, out_index, vectore);
        unlink(index.c_str());
        unlink(values.c_str());
        rename(out_filename.c_str(), values.c_str());
        rename(out_index.c_str(), index.c_str());
    }

};





#endif //VARSORTER_EXTERNAL_MERGE_SORT_H

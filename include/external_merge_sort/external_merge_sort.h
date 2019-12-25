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
#include "original/serializer_with_sort.h"
#include "original/smart_malloc.h"
#include "quicksort.h"

struct void_virtual_sorter : public  virtual_sorter {
    void_virtual_sorter();
    int compare(void *lhs, uint_fast64_t lhs_size, void *rhs, uint_fast64_t rhs_size) override;
};

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
    bool isFixedSize, useSecondaryIndex;
    uint_fast64_t data_block_fixed_size;

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
        void_virtual_sorter opener;
        size_t num_ways, full;
        size_t completed = 0;

        // Determining the size of the chunks, and the number of the file chunks
        if (isFixedSize) {
            opener.openvirtual_sorter(data_block_fixed_size, input_file);
            full = (opener.data_serialized_file / data_block_fixed_size);
            num_ways = (opener.data_serialized_file / data_block_fixed_size) / run_size;
        } else {
            opener.openvirtual_sorter(input_index_file, input_file);
            full = (opener.struct_primary_index_size / (sizeof(struct primary_index)));
            num_ways = full / run_size;
        }

        // output scratch files;
        out.reserve(num_ways);
        for (size_t i = 0; i < num_ways; i++)
        {
            // Creating the i-th file
            std::string filename_value = std::to_string(i) + ".value";

            if (isFixedSize) {
                // Open output files in write mode.
                out.emplace_back(data_block_fixed_size, filename_value, useSecondaryIndex);
                out.rbegin()->sorter = new void_virtual_sorter();
            } else {
                std::string filename_index = std::to_string(i) + ".idx";
                // Open output files in write mode, alongside with the secondary index information
                out.emplace_back(filename_index, filename_value, useSecondaryIndex);
                out.rbegin()->sorter = new void_virtual_sorter();
            }
        }

        // allocate a dynamic array large enough
        // to accommodate runs of size run_size
        std::vector<smart_malloc> in_memory_file_copy{(size_t)run_size};

        bool more_input = true;
        int next_output_file = 0;

        int i;
        // Scanning the whole file: opening the scanner.
        virtual_sorter::iterator ptr = opener.begin(), fini = opener.end();
        while (completed != full) {
            // write run_size elements into arr from input file
            int currentSize = 0;
            for (i = 0; i < run_size; i++) {
                // Terminate the iteration if I finished to scan the file
                if (ptr == fini || ptr->iov_base == nullptr) {
                    break;
                } else {
                    if (!next_output_file) {
                        in_memory_file_copy.emplace_back();
                    }
                    in_memory_file_copy[i].id = ptr.getCurrentPosition();
                    in_memory_file_copy[i].docopy(*ptr);
                    completed++;
                    ptr++;
                    currentSize++;
                }
            }

            // TODO: in parallel
            q.do_quicksort(in_memory_file_copy, currentSize);

            // write the records to the appropriate scratch output file
            // can't assume that the loop runs to run_size
            // since the last run's length may be less than run_size
            for (int j = 0; j < currentSize; j++) {
                // in the meantime, writing the secondary memory index
                out[next_output_file].insert(in_memory_file_copy[j].malloced_iovec, &in_memory_file_copy[j].id);
            }
            //out[next_output_file].close();
            next_output_file++;
        }

        // Forcing to free the memory
        for (smart_malloc& toReset : in_memory_file_copy)
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
        inserter out(false);

        if (isFixedSize) {
            out.open(data_block_fixed_size, out_filename);
        } else
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
            //std::cout << std::string((const char*)x.iovec.iov_base, x.iovec.iov_len) << std::endl;
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
            ptr.unlink();
        }

        out.close();
    }


public:

    external_merge_sort(bool use_secondary) : isFixedSize{false}, data_block_fixed_size{0}, useSecondaryIndex{use_secondary}
    { }

    external_merge_sort(uint_fast64_t data_block, bool use_secondary) : data_block_fixed_size{data_block}, isFixedSize{true}, useSecondaryIndex{use_secondary}
    { }

    /**
     * Before running the method, ensure that these two files are closed
     * @param values_path
     * @param primary_index_path
     * @param run_size
     */
    void run(std::string& values_path, std::string& primary_index_path, size_t run_size) {


        std::vector<serializer_with_sort> splitted_sorted_files;
        createInitialRuns(values_path, primary_index_path, run_size, splitted_sorted_files);

        std::string out_filename{"temp.txt"};
        std::string out_index{"temp.idx"};
        mergeFiles(out_filename, out_index, splitted_sorted_files);

        unlink(primary_index_path.c_str());
        unlink(values_path.c_str());
        rename(out_filename.c_str(), values_path.c_str());
        rename(out_index.c_str(), primary_index_path.c_str());
    }

};





#endif //VARSORTER_EXTERNAL_MERGE_SORT_H

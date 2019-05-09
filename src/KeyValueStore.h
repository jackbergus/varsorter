/*
 * KeyValueStore.h
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
// Created by giacomo on 01/05/19.
//

#ifndef VARSORTER_KEYVALUESTORE_H
#define VARSORTER_KEYVALUESTORE_H


#include "original/virtual_key_value_store.h"

/**
 * This KeyValue store has a specific policy for using external_merge_sort whenever the elements became too big for
 * using a secondary-memory via memory mapping sorting
 *
 * @tparam QuicksortLessComparatorForKeys
 */
template <typename QuicksortLessComparatorForKeys> class KeyValueStore : public virtual_key_value_store {
    QuicksortLessComparatorForKeys keyComparator;
    external_merge_sort<QuicksortLessComparatorForKeys> ems;

public:
    size_t bigThreshold = 10000000;
    size_t runSize =      1000000;
    KeyValueStore(const std::string &indexFile, const std::string &valuesFile) : virtual_key_value_store(indexFile,
                                                                                                        valuesFile) {}

    KeyValueStore(uint_fast64_t fixed_size, const std::string &valuesFile) : virtual_key_value_store(fixed_size,
                                                                                                         valuesFile) {}

    int compareKeys(void *lhs, uint_fast64_t lhs_size, void *rhs, uint_fast64_t rhs_size) {
        keyComparator.compare(lhs, lhs_size,rhs, rhs_size);
    }

    void sortPair() {
        if ((struct_index_size / sizeof(struct index)-1) > bigThreshold) {
            c.close();
            ems.run(this->bulkFile, this->indexFile, runSize);
            openvirtual_sorter(indexFile, bulkFile);
        } else {
            //printIndex();
            quicksort(0, (virtual_sorter::isFixedSize ? (data_serialized_file / virtual_sorter::fixed_size -1) : (struct_index_size / sizeof(struct index)-1)));
            // I have to reorder the external memory after reordering the index if and only if the memory was not already in-place
            // sorted, 'cause the data structure is bounded
            if (!virtual_sorter::isFixedSize) {
                std::string sf = bulkFile + ".sorted";
                FILE *f = fopen(sf.c_str(), "w+");
                // pointer keeping update of how many elements have been written so far.
                uint_fast64_t init = 0;
                for (uint_fast64_t i = 0; i < struct_index_size / sizeof(struct index); i++) {
                    // Ordering the data element according to the index ordering, that is writing in the first position the
                    // element in the first position of the index, which is now the smallest element.
                    fwrite(mmap_kv_File + mmap_index_File[i].begin, mmap_index_File[i].end - mmap_index_File[i].begin, 1, f);
                    // updating the index data structure
                    mmap_index_File[i].end = mmap_index_File[i].end - mmap_index_File[i].begin + init;
                    mmap_index_File[i].begin = init;
                    init = mmap_index_File[i].end;
                }
                fclose(f);
                // Closing the old unsorted file
                close(fdkvf);
                munmap(mmap_kv_File, data_serialized_file);
                fdkvf = 0;
                rename(sf.c_str(), bulkFile.c_str());
                //printIndex();
                mmap_kv_File = (char *) mmapFile(bulkFile, &data_serialized_file, &fdkvf);
            } // else, the memory has been already swapped
        }

    }

};


#endif //VARSORTER_KEYVALUESTORE_H

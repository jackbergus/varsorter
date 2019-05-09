/*
 * virtual_sorter.h
 * This file is part of varsorter
 *
 * Copyright (C) 2019 - Giacomo Bergami
 * 
 * Created on 10/08/17
 *
 * varsorter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * varsorter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with varsorter. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VARSORTER_VIRTUAL_SORTER_H
#define VARSORTER_VIRTUAL_SORTER_H

#include <iostream>
#include <cstring>

#include "mmapFile.h"
#include "index.h"
#include "java_utils.h"
#include "smart_index_pointer.h"
#include "smart_malloc.h"

class virtual_sorter {
protected:
    int fdmif;

    char* mmap_kv_File = nullptr;
    int fdkvf;

    bool isOpen = false;
    std::string bulkFile, indexFile;

    smart_index_pointer ptr_arr;
    smart_malloc ptr_mem_tmp;


    uint_fast64_t partition(const uint_fast64_t left, const uint_fast64_t right);
    void quicksort(uint_fast64_t left, uint_fast64_t right);

public:
    struct index* mmap_index_File = nullptr;
    uint_fast64_t struct_index_size;
    uint_fast64_t data_serialized_file;
    bool isFixedSize;
    uint_fast64_t fixed_size;

    virtual_sorter();
    void openvirtual_sorter(std::string indexFile, std::string kvFile);
    void openvirtual_sorter(uint_fast64_t fixed_size, std::string kvFile);
    void doclose();

    /**
     * This function compares the values, probably key and value, that are serialized in secondary memory.
     *
     * @param lhs           Pointer to the left memory address
     * @param lhs_size      Size of the left memory
     * @param rhs           Pointer to the right memory address
     * @param rhs_size      Size of the right memory
     * @return              Standard int comparison value (0 equiv, 1 greater, -1 less)
     */
    virtual int compare(void* lhs, uint_fast64_t lhs_size, void* rhs, uint_fast64_t rhs_size) = 0;
    void sortPair();
    ~virtual_sorter();
    void printIndex();

    class iterator {

        struct index* mmap_index_File;
        uint_fast64_t last_element_pos;
        uint_fast64_t curr_vect_pos;
        char* mmap_kv_File;
        struct iovec current;
        bool isFixedSize;
        uint_fast64_t fixed_size;

    public:
        /**
         * Implementation of the default empty iterator
         */
        iterator();

        /**
         * Iterator of a secondary memory object having no fixed size struture, and therefore requiring a
         * @param mmap_index_File
         * @param end_index
         * @param index
         * @param mmap_kv_File
         */
        iterator(struct index *mmap_index_File, uint_fast64_t end_index, uint_fast64_t index, char *mmap_kv_File);


        iterator(uint_fast64_t data_block_size, uint_fast64_t end_index, uint_fast64_t index, char *mmap_kv_File);

        /**
         * Copying an iterator
         * @param cp
         */
        iterator(const iterator& cp);
        iterator& operator=(const iterator& elem);
        bool operator==(const iterator &rhs) const;
        bool operator!=(const iterator &rhs) const;
        struct iovec* operator->();
        struct iovec* operator*();
        iterator& operator++();
        iterator operator++(int);
        void setNewLen(uint_fast64_t i);
        void updateWith(uint_fast64_t offsetBegin, uint_fast64_t newLen);
        void getKey(iovec& key);
        void getKey(uint_fast64_t& key);
        void getValue(iovec &iovec);
        void getValue(uint_fast64_t& key);
    };

    iterator binSearch(void* tmp, uint_fast64_t size);
    iterator begin() const;
    iterator end() const;

    void risk_overwrite(void *i, void *pVoid, uint_fast64_t i1);
    void openIfRequired(std::string indexFile, std::string kvFile);
    void openIfRequired(uint_fast64_t fixed_size, std::string kvFile);
};


#endif //VARSORTER_VIRTUAL_SORTER_H

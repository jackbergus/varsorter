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

class virtual_sorter {
protected:
    int fdmif;

    char* mmap_kv_File;
    int fdkvf;

    bool isOpen = false;
    std::string bulkFile, indexFile;

    uint_fast64_t partition(struct index *arr, const uint_fast64_t left, const uint_fast64_t right);
    void quicksort(struct index *arr, uint_fast64_t left, uint_fast64_t right);

public:
    struct index* mmap_index_File;
    uint_fast64_t struct_index_size;
    uint_fast64_t data_serialized_file;

    virtual_sorter();
    void openvirtual_sorter(std::string indexFile, std::string kvFile);
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
    int compare(void* lhs, uint_fast64_t lhs_size, void* rhs, uint_fast64_t rhs_size) { return 0; };
    void sortPair();
    ~virtual_sorter();
    void printIndex();

    class iterator {
        struct index* mmap_index_File;
        uint_fast64_t end_index;
        uint_fast64_t index;
        char* mmap_kv_File;
        struct iovec current;

    public:
        iterator();
        iterator(struct index *mmap_index_File, uint_fast64_t end_index, uint_fast64_t index, char *mmap_kv_File);
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
        void getValue(iovec &iovec);
    };

    iterator binSearch(void* tmp, uint_fast64_t size);
    iterator begin() const;
    iterator end() const;

    void risk_overwrite(void *i, void *pVoid, uint_fast64_t i1);
    void openIfRequired(std::string indexFile, std::string kvFile);
};


#endif //VARSORTER_VIRTUAL_SORTER_H

/*
 * virtual_sorter.cpp
 * This file is part of varsorter
 *
 * Copyright (C) 2017 - Giacomo Bergami
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


#include "virtual_sorter.h"

#define swap(p,left) \
     do {\
         if (isFixedSize) {\
            uint_fast64_t aib = ptr_arr[p]->begin;\
            uint_fast64_t aie = ptr_arr[p]->end;\
            ptr_arr[p]->begin = ptr_arr[left]->begin;\
            ptr_arr[p]->end = ptr_arr[left]->end;\
            ptr_arr[left]->begin = aib;\
            ptr_arr[left]->end = aie;\
        } else {\
            ptr_arr.unsafe_swap(p, left, mmap_kv_File);\
        } \
    }while(0)\

#define END_SIZE            (isFixedSize ? (data_serialized_file / fixed_size -1) : (struct_index_size / sizeof(struct index)-1))
#define OBJECT_POINTER(m)   (isFixedSize ? fixed_size * m : mmap_index_File[m].begin)
#define OBJECT_SIZE(m)      (isFixedSize ? fixed_size : mmap_index_File[m].end-mmap_index_File[m].begin)

uint_fast64_t virtual_sorter::partition(const uint_fast64_t left, const uint_fast64_t right) {
    auto ptr_arr_right = ptr_arr[right];
    void* pivotElement = mmap_kv_File+ptr_arr_right->begin;
    uint_fast64_t pivotElementSize = ptr_arr_right->end - ptr_arr_right->begin;
    uint_fast64_t i = left - 1;
    for (uint_fast64_t j = left; j < right; j++) {
        auto ptr_arr_j = ptr_arr[j];
        if (compare(mmap_kv_File+ptr_arr_j->begin, ptr_arr_j->end - ptr_arr_j->begin, pivotElement, pivotElementSize) < 0) {
            i++;
            swap(i,j);
        }
    }
    auto ptr_arr_ipp = ptr_arr[i+1];
    if (compare(mmap_kv_File+ptr_arr_right->begin, ptr_arr_right->end - ptr_arr_right->begin, mmap_kv_File+ptr_arr_ipp->begin, ptr_arr_ipp->end - ptr_arr_ipp->begin) < 0) {
        swap((i+1),right);
    }

    return i+1;
}

void virtual_sorter::quicksort( uint_fast64_t left, uint_fast64_t right) {
    if (left < right) {
        uint_fast64_t part = partition(left, right);
        if (part > 0) quicksort(left, part - 1);
        quicksort(part + 1, right);
    }
}

void virtual_sorter::printIndex() {
    for (uint_fast64_t i = 0; i< 1+END_SIZE; i++) {
        std::cout << mmap_index_File[i].begin << "-->" << mmap_index_File[i].end << std::endl;
    }
}


void virtual_sorter::sortPair() {
    //printIndex();
    quicksort(0, END_SIZE);
    // I have to reorder the external memory after reordering the index if and only if the memory was not already in-place
    // sorted, 'cause the data structure is bounded
    if (!isFixedSize) {
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

virtual_sorter::~virtual_sorter() {
    if (fdmif) {
        close(fdmif);
        munmap(mmap_index_File, struct_index_size);
        fdmif = 0;
    }
    if (fdkvf) {
        close(fdkvf);
        munmap(mmap_kv_File, data_serialized_file);
        fdkvf = 0;
    }
}
void virtual_sorter::openIfRequired(std::string indexFile, std::string kvFile) {
    if (!isOpen) {
        doclose();
        bulkFile = kvFile;
        this->indexFile = indexFile;
        mmap_kv_File = (char*)mmapFile(kvFile, &data_serialized_file, &fdkvf);
        mmap_index_File = (struct index*)mmapFile(indexFile, &struct_index_size, &fdmif);
        this->ptr_arr.open(mmap_index_File);
        isOpen = true;
    }
}

void virtual_sorter::openIfRequired(uint_fast64_t fixed_size, std::string kvFile) {
    if (!isOpen) {
        doclose();
        if (this->fixed_size != fixed_size)
            ptr_mem_tmp.domalloc(fixed_size);
        bulkFile = kvFile;
        this->indexFile = indexFile;
        mmap_kv_File = (char*)mmapFile(kvFile, &data_serialized_file, &fdkvf);
        mmap_index_File = nullptr;
        this->fixed_size = fixed_size;
        this->ptr_arr.open(fixed_size);
        isOpen = true;
    }
}

void virtual_sorter::openvirtual_sorter(std::string indexFile, std::string kvFile) {
    if (isOpen) {
        doclose();
    }
    isFixedSize = false;
    bulkFile = kvFile;
    this->indexFile = indexFile;
    mmap_kv_File = (char*)mmapFile(kvFile, &data_serialized_file, &fdkvf);
    mmap_index_File = (struct index*)mmapFile(indexFile, &struct_index_size, &fdmif);
    this->ptr_arr.open(mmap_index_File);
    isOpen = true;
}


void virtual_sorter::openvirtual_sorter(uint_fast64_t fixed_size, std::string kvFile) {
    if (isOpen) {
        doclose();
    }
    if (this->fixed_size != fixed_size)
        ptr_mem_tmp.domalloc(fixed_size);
    isFixedSize = true;
    this->fixed_size = fixed_size;
    this->ptr_arr.open(fixed_size);
    bulkFile = kvFile;
    mmap_kv_File = (char*)mmapFile(kvFile, &data_serialized_file, &fdkvf);
    mmap_index_File = nullptr;
    isOpen = true;
}

void virtual_sorter::doclose() {
    if (isOpen) {
        if (fdmif) {
            close(fdmif);
            munmap(mmap_index_File, struct_index_size);
            mmap_index_File = nullptr;
            struct_index_size = 0;
            fdmif = 0;
        }
        if (fdkvf) {
            close(fdkvf);
            munmap(mmap_kv_File, data_serialized_file);
            mmap_kv_File = nullptr;
            data_serialized_file = 0;
            fdkvf = 0;
        }
        bulkFile.clear();
        isOpen = false;
    }
}

void virtual_sorter::risk_overwrite(void *i, void *pVoid, uint_fast64_t i1) {
    memcpy(i, pVoid, i1);
}

virtual_sorter::virtual_sorter() {
    struct_index_size = 0;
    mmap_kv_File = nullptr;
    fdkvf = 0;
    fdmif = 0;
    mmap_index_File = nullptr;
    data_serialized_file = 0;
    bulkFile = "";
    isOpen = false;
}


virtual_sorter::iterator virtual_sorter::binSearch(void *tmp, uint_fast64_t size) {
    const uint_fast64_t fini = END_SIZE;
    uint_fast64_t i = fini;
    uint_fast64_t j = 0;
    while (i>=j) {
        uint_fast64_t m = (i+j)/2;
        uint_fast64_t leftOffset = OBJECT_POINTER(m);
        uint_fast64_t leftSize = OBJECT_SIZE(m);
        int cr = compare(mmap_kv_File+leftOffset, leftSize, tmp, size);
        if (cr==0) {
            if (isFixedSize)
                return iterator{fixed_size, fini, m, mmap_kv_File};
            else
                return iterator{mmap_index_File, fini, m, mmap_kv_File};
        }
        else if (cr<0) {
            j = m+1;
        } else {
            i = m-1;
        }
    }
    return end();
}

virtual_sorter::iterator virtual_sorter::begin() const {
    if (isFixedSize)
        return iterator(fixed_size, END_SIZE, 0, mmap_kv_File);
    else
        return iterator(mmap_index_File, END_SIZE, 0, mmap_kv_File);
}

virtual_sorter::iterator virtual_sorter::end() const {
    if (isFixedSize)
        return iterator(fixed_size, END_SIZE, 1+END_SIZE, mmap_kv_File);
    else
        return iterator(mmap_index_File, END_SIZE, 1+END_SIZE, mmap_kv_File);
}

/*

  ___  _                    _
 |_ _|| |_  ___  _ _  __ _ | |_  ___  _ _
  | | |  _|/ -_)| '_|/ _` ||  _|/ _ \| '_|
 |___| \__|\___||_|  \__,_| \__|\___/|_|


 */


virtual_sorter::iterator::iterator(struct index *mmap_index_File, uint_fast64_t end_index, uint_fast64_t index,
                                   char *mmap_kv_File)
                                   : mmap_index_File(mmap_index_File), last_element_pos(end_index),
                                                         curr_vect_pos(index), mmap_kv_File(mmap_kv_File) {
    isFixedSize = false;
    fixed_size = 0UL;
}

virtual_sorter::iterator::iterator(uint_fast64_t data_block_size, uint_fast64_t end_index, uint_fast64_t index,
                                   char *mmap_kv_File)
        : mmap_index_File(nullptr), last_element_pos(end_index),
          curr_vect_pos(index), mmap_kv_File(mmap_kv_File) {
    isFixedSize = true;
    fixed_size = data_block_size;
}


virtual_sorter::iterator::iterator() {
    mmap_index_File = nullptr;
    last_element_pos = 0;
    curr_vect_pos = 0;
    mmap_kv_File = nullptr;
    memset((void*)&current,0,sizeof(current));
    isFixedSize = true;
    fixed_size = 0UL;
}


bool virtual_sorter::iterator::operator==(const virtual_sorter::iterator &rhs) const {
    return mmap_index_File == rhs.mmap_index_File &&
           last_element_pos == rhs.last_element_pos &&
           curr_vect_pos == rhs.curr_vect_pos &&
           mmap_kv_File == rhs.mmap_kv_File &&
           (isFixedSize != rhs.isFixedSize || rhs.fixed_size == fixed_size);
}

bool virtual_sorter::iterator::operator!=(const virtual_sorter::iterator &rhs) const {
    return !(rhs == *this);
}

virtual_sorter::iterator& virtual_sorter::iterator::operator=(const virtual_sorter::iterator& elem) {
    mmap_index_File = elem.mmap_index_File;
    last_element_pos = elem.last_element_pos;
    curr_vect_pos = elem.curr_vect_pos;
    mmap_kv_File = elem.mmap_kv_File;
    current.iov_base = elem.current.iov_base;
    current.iov_len = elem.current.iov_len;
    isFixedSize = elem.isFixedSize;
    fixed_size = elem.fixed_size;
}

virtual_sorter::iterator::iterator(const virtual_sorter::iterator &cp)  {
    mmap_index_File = cp.mmap_index_File;
    last_element_pos = cp.last_element_pos;
    curr_vect_pos = cp.curr_vect_pos;
    mmap_kv_File = cp.mmap_kv_File;
    current.iov_base = cp.current.iov_base;
    current.iov_len = cp.current.iov_len;
    isFixedSize = cp.isFixedSize;
    fixed_size = cp.fixed_size;
}

void virtual_sorter::iterator::setNewLen(uint_fast64_t i) {
    if (!isFixedSize)
        mmap_index_File[curr_vect_pos].end = mmap_index_File[curr_vect_pos].begin + i;
    else
        std::cerr << "[setNewLen] Warning: when data is on fixed size, I would not need to resize the data underneath" << std::endl;
}

void virtual_sorter::iterator::updateWith(uint_fast64_t offsetBegin, uint_fast64_t newLen) {
    if (!isFixedSize) {
        mmap_index_File[curr_vect_pos].begin = offsetBegin;
        mmap_index_File[curr_vect_pos].end = mmap_index_File[curr_vect_pos].begin + newLen;
    } else {
        std::cerr << "[updateWith] Warning: when data is on fixed size, I would not need to resize the data underneath" << std::endl;
    }
}

void virtual_sorter::iterator::getKey(iovec& key) {
    key.iov_len = ((uint_fast64_t*)(mmap_kv_File + OBJECT_POINTER(curr_vect_pos)))[0];
    key.iov_base = (void*)(mmap_kv_File + OBJECT_POINTER(curr_vect_pos) + sizeof(uint_fast64_t)*2);
}

void virtual_sorter::iterator::getValue(iovec &value) {
    value.iov_len = ((uint_fast64_t*)(mmap_kv_File + OBJECT_POINTER(curr_vect_pos)))[1];
    value.iov_base = (void*)(mmap_kv_File + OBJECT_POINTER(curr_vect_pos) + sizeof(uint_fast64_t)*2 +((uint_fast64_t*)(mmap_kv_File + OBJECT_POINTER(curr_vect_pos)))[0]);
}


void virtual_sorter::iterator::getValue(uint_fast64_t &value) {
    if ( ((uint_fast64_t*)(mmap_kv_File + OBJECT_POINTER(curr_vect_pos)))[1] == sizeof(uint_fast64_t))
        value = *(uint_fast64_t*)(mmap_kv_File + OBJECT_POINTER(curr_vect_pos) + sizeof(uint_fast64_t)*2 +((uint_fast64_t*)(mmap_kv_File + OBJECT_POINTER(curr_vect_pos)))[0]);
}

void virtual_sorter::iterator::getKey(uint_fast64_t &key) {
    if (((uint_fast64_t*)(mmap_kv_File + OBJECT_POINTER(curr_vect_pos)))[0] == sizeof(uint_fast64_t))
        key = *(uint_fast64_t*)(mmap_kv_File + OBJECT_POINTER(curr_vect_pos) + sizeof(uint_fast64_t)*2);
}


struct iovec *virtual_sorter::iterator::operator->() {
    if (curr_vect_pos > this->last_element_pos) {
        current.iov_len = 0;
        current.iov_base = nullptr;
    } else {
        current.iov_base = (mmap_kv_File + OBJECT_POINTER(curr_vect_pos));
        current.iov_len = OBJECT_SIZE(curr_vect_pos);
    }
    return &current;
}

struct iovec *virtual_sorter::iterator::operator*() {
    if (curr_vect_pos > this->last_element_pos) {
        current.iov_len = 0;
        current.iov_base = nullptr;
    } else {
        current.iov_base = (mmap_kv_File + OBJECT_POINTER(curr_vect_pos));
        current.iov_len = OBJECT_SIZE(curr_vect_pos);
    }
    return &current;
}

virtual_sorter::iterator &virtual_sorter::iterator::operator++() {
    if (curr_vect_pos < last_element_pos) {
        curr_vect_pos++;
    }
    return *this;
}

virtual_sorter::iterator virtual_sorter::iterator::operator++(int) {
    uint_fast64_t currStep = curr_vect_pos++;
    if (isFixedSize)
        return iterator{fixed_size, last_element_pos, currStep, mmap_kv_File};
    else
        return iterator{mmap_index_File, last_element_pos, currStep, mmap_kv_File};
}

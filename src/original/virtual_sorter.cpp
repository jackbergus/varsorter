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

#define swap(p,left) {\
        uint_fast64_t aib = arr[p].begin;\
        uint_fast64_t aie = arr[p].end;\
        arr[p].begin = arr[left].begin;\
        arr[p].end = arr[left].end;\
        arr[left].begin = aib;\
        arr[left].end = aie;\
    }\

uint_fast64_t virtual_sorter::partition(struct index *arr, const uint_fast64_t left, const uint_fast64_t right) {
    void* pivotElement = mmap_kv_File+arr[right].begin;
    uint_fast64_t pivotElementSize = arr[right].end - arr[right].begin;
    uint_fast64_t i = left - 1;
    for (uint_fast64_t j = left; j < right; j++) {
        if (compare(mmap_kv_File+arr[j].begin, arr[j].end - arr[j].begin, pivotElement, pivotElementSize) < 0) {
            i++;
            swap(i,j);
        }
    }
    if (compare(mmap_kv_File+arr[right].begin, arr[right].end - arr[right].begin, mmap_kv_File+arr[i+1].begin, arr[i+1].end - arr[i+1].begin) < 0) {
        swap((i+1),right);
    }

    return i+1;
}

void virtual_sorter::quicksort(struct index *arr, uint_fast64_t left, uint_fast64_t right) {
    if (left < right) {
        uint_fast64_t part = partition(arr, left, right);
        if (part > 0) quicksort(arr, left, part - 1);
        quicksort(arr, part + 1, right);
    }
}

void virtual_sorter::printIndex() {
    for (uint_fast64_t i = 0; i< struct_index_size / sizeof(struct index); i++) {
        std::cout << mmap_index_File[i].begin << "-->" << mmap_index_File[i].end << std::endl;
    }
}

void virtual_sorter::sortPair() {
    //printIndex();
    quicksort(mmap_index_File, 0, struct_index_size / sizeof(struct index)-1);
    std::string sf = bulkFile+".sorted";
    FILE* f = fopen(sf.c_str(),"w+");
    uint_fast64_t init = 0;
    for (uint_fast64_t i = 0; i< struct_index_size / sizeof(struct index); i++) {
        fwrite(mmap_kv_File+mmap_index_File[i].begin, mmap_index_File[i].end-mmap_index_File[i].begin, 1, f);
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
    mmap_kv_File = (char*)mmapFile(bulkFile, &data_serialized_file, &fdkvf);
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
        isOpen = true;
    }
}

void virtual_sorter::openvirtual_sorter(std::string indexFile, std::string kvFile) {
    if (isOpen) {
        doclose();
    }
    bulkFile = kvFile;
    this->indexFile = indexFile;
    mmap_kv_File = (char*)mmapFile(kvFile, &data_serialized_file, &fdkvf);
    mmap_index_File = (struct index*)mmapFile(indexFile, &struct_index_size, &fdmif);
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

virtual_sorter::iterator virtual_sorter::binSearch(void *tmp, uint_fast64_t size) {
    uint_fast64_t i = struct_index_size / sizeof(struct index)-1;
    uint_fast64_t j = 0;
    while (i>=j) {
        uint_fast64_t m = (i+j)/2;
        int cr = compare(mmap_kv_File+mmap_index_File[m].begin, mmap_index_File[m].end-mmap_index_File[m].begin, tmp, size);
        if (cr==0)
            return iterator{mmap_index_File, struct_index_size / sizeof(struct index)-1, m, mmap_kv_File};
        else if (cr<0) {
            j = m+1;
        } else {
            i = m-1;
        }
    }
    return end();
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

virtual_sorter::iterator virtual_sorter::begin() const {
    return iterator(mmap_index_File, struct_index_size / sizeof(struct index) - 1, 0, mmap_kv_File);
}

virtual_sorter::iterator virtual_sorter::end() const {
    return iterator(mmap_index_File, struct_index_size / sizeof(struct index) - 1, struct_index_size / sizeof(struct index), mmap_kv_File);
}

virtual_sorter::iterator::iterator(struct index *mmap_index_File, uint_fast64_t end_index, uint_fast64_t index,
                                   char *mmap_kv_File) : mmap_index_File(mmap_index_File), end_index(end_index),
                                                         index(index), mmap_kv_File(mmap_kv_File) {}

bool virtual_sorter::iterator::operator==(const virtual_sorter::iterator &rhs) const {
    return mmap_index_File == rhs.mmap_index_File &&
           end_index == rhs.end_index &&
           index == rhs.index &&
           mmap_kv_File == rhs.mmap_kv_File;
}

bool virtual_sorter::iterator::operator!=(const virtual_sorter::iterator &rhs) const {
    return !(rhs == *this);
}

virtual_sorter::iterator& virtual_sorter::iterator::operator=(const virtual_sorter::iterator& elem) {
    mmap_index_File = elem.mmap_index_File;
    end_index = elem.end_index;
    index = elem.index;
    mmap_kv_File = elem.mmap_kv_File;
}

virtual_sorter::iterator::iterator(const virtual_sorter::iterator &cp) : iterator{cp.mmap_index_File, cp.end_index, cp.index, cp.mmap_kv_File} {}

void virtual_sorter::iterator::setNewLen(uint_fast64_t i) {
    mmap_index_File[index].end = mmap_index_File[index].begin + i;
}

void virtual_sorter::iterator::updateWith(uint_fast64_t offsetBegin, uint_fast64_t newLen) {
    mmap_index_File[index].begin = offsetBegin;
    mmap_index_File[index].end = mmap_index_File[index].begin + newLen;
}

void virtual_sorter::iterator::getKey(iovec& key) {
    key.iov_len = ((uint_fast64_t*)(mmap_kv_File + mmap_index_File[index].begin))[0];
    key.iov_base = (void*)(mmap_kv_File + mmap_index_File[index].begin + sizeof(uint_fast64_t)*2);
}

void virtual_sorter::iterator::getValue(iovec &value) {
    value.iov_len = ((uint_fast64_t*)(mmap_kv_File + mmap_index_File[index].begin))[1];
    value.iov_base = (void*)(mmap_kv_File + mmap_index_File[index].begin + sizeof(uint_fast64_t)*2 +((uint_fast64_t*)(mmap_kv_File + mmap_index_File[index].begin))[0]);
}

//// ITERATOR

virtual_sorter::iterator::iterator() {
    mmap_index_File = nullptr;
    end_index = 0;
    index = 0;
    mmap_kv_File = nullptr;
    memset((void*)&current,0,sizeof(current));
}

struct iovec *virtual_sorter::iterator::operator->() {
    if (index > this->end_index) {
        current.iov_len = 0;
        current.iov_base = nullptr;
    } else {
        current.iov_base = (mmap_kv_File + mmap_index_File[index].begin);
        current.iov_len = mmap_index_File[index].end -mmap_index_File[index].begin;
    }
    return &current;
}

struct iovec *virtual_sorter::iterator::operator*() {
    if (index > this->end_index) {
        current.iov_len = 0;
        current.iov_base = nullptr;
    } else {
        current.iov_base = (mmap_kv_File + mmap_index_File[index].begin);
        current.iov_len = mmap_index_File[index].end -mmap_index_File[index].begin;
    }
    return &current;
}

virtual_sorter::iterator &virtual_sorter::iterator::operator++() {
    if (index < end_index) {
        index++;
    }
    return *this;
}

virtual_sorter::iterator virtual_sorter::iterator::operator++(int) {
    uint_fast64_t currStep = index++;
    return iterator{mmap_index_File, end_index, currStep, mmap_kv_File};
}

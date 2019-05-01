/*
 * virtual_key_value_store.cpp
 * This file is part of varsorter
 *
 * Copyright (C) 2017 - Giacomo Bergami
 * 
 * Created on 11/08/17
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


#include "virtual_key_value_store.h"

virtual_key_value_store::virtual_key_value_store(const std::string &indexFile, const std::string &valuesFile) :
        serializer_with_sort(indexFile, valuesFile), virtual_sorter{} {
    this->sorter = this;
}

int virtual_key_value_store::compare(void *lhs, uint_fast64_t lhs_size, void *rhs, uint_fast64_t rhs_size) {
    auto* lptr = (uint_fast64_t*)lhs;
    auto* rptr = (uint_fast64_t*)rhs;
    return compareKeys(
            (char*)lhs+(sizeof(uint_fast64_t)*2),
            lptr[0],
            (char*)rhs+(sizeof(uint_fast64_t)*2),
            rptr[0]
    );
}

virtual_sorter::iterator virtual_key_value_store::begin() {
    return dynamic_cast<serializer_with_sort*>(this)->begin();
}

virtual_sorter::iterator virtual_key_value_store::end() {
    return dynamic_cast<serializer_with_sort*>(this)->end();
}

virtual_key_value_store::Deref virtual_key_value_store::operator[](struct iovec &index) {
    return Deref(this, index);
}

virtual_key_value_store::Deref virtual_key_value_store::operator[](std::string &index) {
    return Deref(this, index);
}

// Deref

virtual_key_value_store::Deref::Deref(virtual_key_value_store *a, struct iovec &index) : a(a), key(index.iov_base,index.iov_len) {}

virtual_key_value_store::Deref::Deref(virtual_key_value_store *a, std::string &index) : a(a) {
    key.iov_base = (void *) index.c_str();
    key.iov_len = index.length();
}

void virtual_key_value_store::Deref::operator=(const struct iovec &value) {
    // The final size is the size for the initial offset, and the length of the two values
    size_t keyValMallocd = INITIAL_OFFSET + key.iov_len + value.iov_len;
    // Allocating enough main memory for writing the data
    uint_fast64_t* kvPtr = (uint_fast64_t*)a->malloced.domalloc(keyValMallocd);
    // The first element tells the size of the key
    kvPtr[0] = key.iov_len;
    // The second element tells the size of the value
    kvPtr[1] = value.iov_len;
    // Copies the information in the temporary allocated buffer
    memcpy(((char*)kvPtr)+INITIAL_OFFSET, key.iov_base, key.iov_len);
    memcpy(((char*)kvPtr)+INITIAL_OFFSET+ key.iov_len, value.iov_base, value.iov_len);

    a->update((void*)kvPtr, 0, (void*)kvPtr, keyValMallocd);
    //free(kvPtr);
}

void virtual_key_value_store::Deref::operator=(std::string &value) {
    //setter
    size_t keyValMallocd = INITIAL_OFFSET + key.iov_len + value.length();
    uint_fast64_t* kvPtr = (uint_fast64_t*)a->malloced.domalloc(keyValMallocd);

    kvPtr[0] = key.iov_len;
    kvPtr[1] = value.length();

    memcpy(((char*)kvPtr)+INITIAL_OFFSET, key.iov_base, key.iov_len);
    memcpy(((char*)kvPtr)+INITIAL_OFFSET+ key.iov_len, value.c_str(), value.length());

    a->update((void*)kvPtr, 0, (void*)kvPtr, keyValMallocd);
    //free(kvPtr);
}

void virtual_key_value_store::Deref::operator=(uint_fast64_t &value) {
    //setter
    size_t keyValMallocd = INITIAL_OFFSET + key.iov_len + sizeof(uint_fast64_t);
    uint_fast64_t* kvPtr = (uint_fast64_t*)a->malloced.domalloc(keyValMallocd);


    kvPtr[0] = key.iov_len;
    kvPtr[1] = sizeof(uint_fast64_t);

    memcpy(((char*)kvPtr)+INITIAL_OFFSET, key.iov_base, key.iov_len);
    memcpy(((char*)kvPtr)+INITIAL_OFFSET+ key.iov_len, &value, sizeof(uint_fast64_t));

    a->update((void*)kvPtr, 0, (void*)kvPtr, keyValMallocd);
    //free(kvPtr);
}

virtual_key_value_store::Deref::operator struct iovec *() {
    //getter
    return *a->searchFor(key.iov_base, key.iov_len);
}

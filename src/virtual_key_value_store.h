/*
 * virtual_key_value_store.h
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



#ifndef VARSORTER_VIRTUAL_KEY_VALUE_STORE_H
#define VARSORTER_VIRTUAL_KEY_VALUE_STORE_H

#include <iostream>
#include <cstring>
#include <sstream>

#include "virtual_sorter.h"
#include "serializer_with_sort.h"

#define INITIAL_OFFSET              (sizeof(uint_fast64_t)*2)

#define ABSTRACT_CLASS              class
#define ABSTRACT                    virtual

/**
 * Providing a modern C++ definition of the iovec structure. In particular, this is give with respect to the implementation
 * of the constructor
 */
struct new_iovec {
    /**
     * Pointer to the data to be serialized
     */
    void* iov_base;
    /**
     * Sizoe of the pointed data.
     */
    uint_fast64_t iov_len;
    new_iovec() : iov_base{nullptr}, iov_len{0} {}
    new_iovec(void *memory, uint_fast64_t size);
    new_iovec(int* singleNum) : new_iovec{singleNum, sizeof(int)} {}
    new_iovec(std::string& str) : new_iovec{(void*)str.c_str(), str.length()} {}
    new_iovec(uint_fast64_t* singleNum) : new_iovec{singleNum, sizeof(singleNum)} {}

    // Utility method to use when the data needs to be written in secondary memory
    std::istringstream stream();
};

/**
 * This class allows to store the data in secondary memory by just providing the following semplifications and assumptions:
 * - The data is always serialized by keeping and id and a value associated to that
 * - The [] operator is used to serialize the data (via Deref)
 *
 */
ABSTRACT_CLASS virtual_key_value_store : public virtual_sorter, public serializer_with_sort {
public:
    virtual_key_value_store(const std::string &indexFile, const std::string &valuesFile);

    /**
     * Now the user has only to compare the keys as a definition. All the values are ignored.
     * @param lhs           Pointer to the left key
     * @param lhs_size      Length of the left key
     * @param rhs           Pointer to the right key
     * @param rhs_size      Length of the right key
     * @return
     */
    ABSTRACT int compareKeys(void* lhs, uint_fast64_t lhs_size, void* rhs, uint_fast64_t rhs_size) = 0;

    /**
     * Internal function, that will use the user-defined compareKeys to perform a key comparison
     * @param lhs
     * @param lhs_size
     * @param rhs
     * @param rhs_size
     * @return
     */
    int compare(void* lhs, uint_fast64_t lhs_size, void* rhs, uint_fast64_t rhs_size);

    struct Deref {
        virtual_key_value_store* a;
        struct new_iovec key;
        Deref(virtual_key_value_store* a, struct iovec& index);

        Deref(virtual_key_value_store* a, std::string& index);

        explicit operator struct iovec*() {
            //getter
            return *a->searchFor(key.iov_base, key.iov_len);
        }

        void operator=(const struct iovec& value);

        void operator=(std::string& value);

        void operator=(uint_fast64_t& value);
    };

    iterator begin();

    iterator end();

    Deref operator[](struct iovec& index);

    Deref operator[](std::string& index);

};

#endif //VARSORTER_VIRTUAL_KEY_VALUE_STORE_H

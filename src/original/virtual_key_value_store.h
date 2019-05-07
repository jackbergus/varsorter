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
#include "smart_malloc.h"
#include "java_utils.h"
#include "../../external_merge_sort/external_merge_sort.h"

/**
 * This initial offset is the initial size for storing the key and the value within the same block within the value index
 */
#define INITIAL_OFFSET              (sizeof(uint_fast64_t)*2)


/**
 * This class allows to store the data in secondary memory by just providing the following semplifications and assumptions:
 * - The data is always serialized by keeping and id and a value associated to that
 * - The [] operator is used to serialize the data (via Deref)
 *
 */
ABSTRACT_CLASS virtual_key_value_store : public virtual_sorter, public serializer_with_sort {
    // Destructed by default when destructing the class'object -- freeing the memory
    smart_malloc malloced;

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

    /**
     * This class is the actually the one providing the implementation to the ``vector base'' access abstraction for the
     * key-value store. In particular, it provides both getters and setters for the
     */
    struct Deref {
        virtual_key_value_store* a;
        struct new_iovec key;
        Deref(virtual_key_value_store* a, struct iovec& index);

        Deref(virtual_key_value_store* a, std::string& index);

        /**
         * This is actually the getter: it uses the binary search for getting the required element. Please note that the
         * search
         *
         * @return
         */
        explicit operator struct iovec*();

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

/*
 * string_key_store_example.cpp
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

 

#include "../src/original/virtual_key_value_store.h"

/**
 * Using a custom way to define a comparator, without associating an external comparator.
 */
class string_kvcmp : public virtual_key_value_store {
public:
    string_kvcmp(const std::string &indexFile, const std::string &valuesFile) : virtual_key_value_store(indexFile,
                                                                                                        valuesFile) {}
    int compareKeys(void *lhs, uint_fast64_t lhs_size, void *rhs, uint_fast64_t rhs_size) {
        int toret = strncmp(static_cast<const char *>(lhs), static_cast<const char *>(rhs), lhs_size < rhs_size ? lhs_size : rhs_size);
        if (lhs_size == rhs_size)
            return toret;
        else if (toret == 0) {
            return lhs_size < rhs_size ? -1 : 1;
        } else return toret;
    }
};

int main() {
    unlink("index2.bin");
    unlink("keyvalue.bin");
    string_kvcmp c{"index2.bin","keyvalue.bin"};

    // Using a string as a key
    std::string ciao{"hello"};
    // Using a number as a value
    uint_fast64_t bene = 23;

    // If I have some binary data structure, I can use the iovec structure to provide some data pointer with some
    // pre-determined data size. In this straightforward implementation, it always assumes that the data that is
    // accessed with [] is appended in the bulk insertion
    struct iovec k;
    k.iov_len = ciao.length();
    k.iov_base = (void *) ciao.c_str();
    c[k] = bene;

    std::string come{"continuation"};
    k.iov_len = come.length();
    k.iov_base = (void *) come.c_str();
    bene = 276;
    c[k] = bene;

    // Now the value is a string: please note that this means that the implementation of the serialization is
    // data-representation undependent, as it treats the values as they were always some data sequence
    std::string stai{"are"};
    k.iov_len = stai.length();
    k.iov_base = (void *) stai.c_str();
    std::string etu{"you"};
    c[stai] = etu;

    struct iovec v;
    /*v.iov_len = sizeof(int);
    v.iov_base = &bene;*/
    v.iov_len = etu.length();
    v.iov_base = (void *) etu.c_str();
    c.iovec_multiinsert({k,v,});

    // Sorting the key-value store
    c.sortElement();

    char keyelems[10], valElems[10];
    virtual_sorter::iterator it;
    int count = 0;
    for (it = c.begin(); it != c.end(); it++ ) {
        // Cleaning the chars
        memset(keyelems,0,10);
        memset(valElems,0,10);
        // Copying the pointer of current key and values inside a iovec data structure
        it.getKey(k); it.getValue(v);
        strncpy(keyelems, static_cast<const char *>(k.iov_base), k.iov_len);
        if (!count) {
            // Only the key "are" is the one containing a stirng as a value and, after the sorting, this will be the first key. All the remiaining keys will be numbers...
            strncpy(valElems, static_cast<const char *>(v.iov_base), v.iov_len);
            std::cout << "[[" << keyelems << "~" << valElems << "]]" << std::endl;
        } else {
            // .. In this situation, just print the numbers.
            uint_fast64_t val = *((uint_fast64_t *) v.iov_base);
            std::cout << "[[" << keyelems << "~" << val << "]]" << std::endl;
        }
        count ++;
    }

    // c
    return 0;
}

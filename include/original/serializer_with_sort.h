/*
 * serializer_with_sort.h
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



#ifndef VARSORTER_SERIALIZER_WITH_SORT_H
#define VARSORTER_SERIALIZER_WITH_SORT_H

#include <iostream>
#include <cstring>

#include "new_iovec.h"
#include "virtual_sorter.h"
#include "inserter.h"

#include <initializer_list>

class serializer_with_sort {
    bool hasInserted = false;
    bool hasRiskInsert = false;
    bool hasSorted = false;
    bool isFixedSize_sws = false;
    uint_fast64_t fixed_size = 0;

    /**
     * Updates the old key internally.
     *
     * @param ptr       Pointer to the old data memory location
     * @param neu       Pointer to the new data to serialize
     * @param newLen    Length of the pointed data
     * @return          1 if the old value has been overwritten, and 2 if only the old index has been overwritten but the
     *                  old value appended at the end of the file
     */
    int update_internal(virtual_sorter::iterator &ptr, void *neu, unsigned long newLen,
                        uint_fast64_t *secondary_ptr);

protected:
    std::string index_path;
    std::string values_path;
public:
    inserter c;
    virtual_sorter* sorter = nullptr;
    serializer_with_sort(std::string indexFile, std::string valuesFile, bool use_secondary);
    serializer_with_sort(uint_fast64_t fixed_size, std::string valuesFile, bool use_secondary);
    ~serializer_with_sort();

    bool risk_insert(struct iovec &ptr, uint_fast64_t *secondary_ptr);
    bool risk_insert(void *buff, uint_fast64_t len, uint_fast64_t *secondary_ptr);
    template <typename T> size_t risk_insert(const T& obj) {
        if (hasSorted) {
            sorter->doclose();
            hasSorted = false;
        }
        if (hasInserted) {
            if (!hasRiskInsert) {
                sorter->doclose();
                c.open(index_path, values_path); // fixed_size aware
            }
        } else {
            c.open(index_path, values_path);  // fixed_size aware
        }
        size_t toRet;
        toRet = c.risk_writeKeyAndValue_with_prev<T>(obj, 0);
        hasInserted = true;
        hasRiskInsert = true;
        return toRet;
    }

    void sortElement();
    void unlink();

    bool usesSecondaryIndex() const {
        return c.usesSecondaryIndex();
    }

    bool iovec_multiinsert(std::initializer_list<struct iovec> list);

    virtual_sorter::iterator searchFor(void* buff, uint_fast64_t len);

    /**
     * This method either inserts (old or oldLen are false) or updates the previous entry old with length oldLen with neu and newLen
     *
     *
     * @param old       Old key to be searched for
     * @param oldLen    Length of the key
     * @param neu       New element to be substituted
     * @param newLen    New length to be set
     * @return          0 - the element does not exists = it has been inserted anew
     *                  1 - the element exists = it has been overwritten
     *                  2 - the element exists = it has been inserted anew and the old index has been overwritten
     *                  -1 - error during the insertion
     */
    int update(void *old, uint_fast64_t oldLen, void *neu, uint_fast64_t newLen,
               uint_fast64_t *secondary_ptr);
    int insert(void *ptr, uint_fast64_t len, uint_fast64_t *secondary_ptr);
    int insert(struct new_iovec &x, uint_fast64_t *secondary_ptr);
    template <typename T> size_t insert(const T& obj) {
        return risk_insert<T>(obj);
    }

    virtual_sorter::iterator begin();
    virtual_sorter::iterator end();

    int update(virtual_sorter::iterator &iterator, void *pVoid, unsigned long i);
};


#endif //VARSORTER_SERIALIZER_WITH_SORT_H

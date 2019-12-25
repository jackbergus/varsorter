/*
 * QuicksortLessComparatorKeyValue.h
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

#ifndef VARSORTER_QUICKSORTLESSCOMPARATOR_H
#define VARSORTER_QUICKSORTLESSCOMPARATOR_H

#include "../original/java_utils.h"
#include "../original/smart_malloc.h"
#include "external_merge_sort_example.h"

ABSTRACT_CLASS QuicksortLessComparatorKeyValue {
public:
    ABSTRACT bool compareKeys(void *lhs, uint_fast64_t lhs_size, void *rhs, uint_fast64_t rhs_size) = 0;
    bool greaterThan(void* lhs, size_t leftS, void* rhs, size_t rightS) {
        // size vectors associated to both the key and the value
        auto* lptr = (uint_fast64_t*)lhs;
        auto* rptr = (uint_fast64_t*)rhs;
        // pointing to the key, and setting each key size (from the offset)
        return compareKeys(
                (char*)lhs+(sizeof(uint_fast64_t)*2),
                lptr[0],
                (char*)rhs+(sizeof(uint_fast64_t)*2),
                rptr[0]
        );
    }
    bool operator()(miniheap_iovec &l, miniheap_iovec &r) {
        greaterThan(l.iovec.iov_base, l.iovec.iov_len, r.iovec.iov_base, r.iovec.iov_len);
    };
    bool less(smart_malloc &l, smart_malloc&r) {
        return greaterThan(r.malloced_iovec.iov_base, r.malloced_iovec.iov_len, l.malloced_iovec.iov_base, l.malloced_iovec.iov_len);
    }
};

#endif //VARSORTER_QUICKSORTLESSCOMPARATOR_H

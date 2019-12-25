/*
 * QuicksortLessComparator.h
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

#include "original/java_utils.h"
#include "original/smart_malloc.h"
#include "external_merge_sort.h"

ABSTRACT_CLASS QuicksortLessComparator {
public:
    ABSTRACT bool greaterThan(void* leftM, size_t leftS, void* rightM, size_t rightS) = 0;
    bool lessThan(void* leftM, size_t leftS, void* rightM, size_t rightS) {
        return greaterThan(rightM, rightS, leftM, leftS);
    }
    int compare(void* leftM, size_t leftS, void* rightM, size_t rightS) {
        if (lessThan(leftM, leftS, rightM, rightS))
            return -1;
        else if (greaterThan(leftM, leftS, rightM, rightS))
            return 1;
        else
            return 0;
    }
    bool operator()(miniheap_iovec &l, miniheap_iovec &r) {
        greaterThan(l.iovec.iov_base, l.iovec.iov_len, r.iovec.iov_base, r.iovec.iov_len);
    };
    bool less(smart_malloc &l, smart_malloc&r) {
        return greaterThan(r.malloced_iovec.iov_base, r.malloced_iovec.iov_len, l.malloced_iovec.iov_base, l.malloced_iovec.iov_len);
    }
};

#endif //VARSORTER_QUICKSORTLESSCOMPARATOR_H

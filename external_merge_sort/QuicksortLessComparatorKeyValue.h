//
// Created by giacomo on 01/05/19.
//

#ifndef VARSORTER_QUICKSORTLESSCOMPARATOR_H
#define VARSORTER_QUICKSORTLESSCOMPARATOR_H

#include "../src/original/java_utils.h"
#include "../src/original/smart_malloc.h"
#include "external_merge_sort.h"

ABSTRACT_CLASS QuicksortLessComparator {
public:
    ABSTRACT bool greaterThan(void* leftM, size_t leftS, void* rightM, size_t rightS) = 0;
    bool operator()(miniheap_iovec &l, miniheap_iovec &r) {
        greaterThan(l.iovec.iov_base, l.iovec.iov_len, r.iovec.iov_base, r.iovec.iov_len);
    };
    bool less(smart_malloc &l, smart_malloc&r) {
        return greaterThan(r.malloced_iovec.iov_base, r.malloced_iovec.iov_len, l.malloced_iovec.iov_base, l.malloced_iovec.iov_len);
    }
};

#endif //VARSORTER_QUICKSORTLESSCOMPARATOR_H

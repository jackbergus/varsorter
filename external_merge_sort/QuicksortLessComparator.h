//
// Created by giacomo on 01/05/19.
//

#ifndef VARSORTER_QUICKSORTCOMPARATOR_H
#define VARSORTER_QUICKSORTCOMPARATOR_H

#include "../src/original/java_utils.h"
#include "../src/original/smart_malloc.h"

ABSTRACT_CLASS QuicksortComparator {
    ABSTRACT bool operator()(smart_malloc &l, smart_malloc &r) = 0;
};

#endif //VARSORTER_QUICKSORTCOMPARATOR_H

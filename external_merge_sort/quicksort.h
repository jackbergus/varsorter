/*
 * quicksort.h
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

#ifndef VARSORTER_QUICKSORT_H
#define VARSORTER_QUICKSORT_H


#include <vector>
#include "../src/original/smart_malloc.h"

template <typename AQuicksortComparator> struct quicksort {
    AQuicksortComparator lessThan{};

    int partition(std::vector<smart_malloc> &arr, const int left, const int right) {
        smart_malloc& pivotElement = arr[right];
        uint_fast64_t i = left - 1;
        for (uint_fast64_t j = left; j < right; j++) {
            if (lessThan.less(arr[j], pivotElement)) {
                i++;
                swap(arr[i],arr[j]);
            }
        }
        if (lessThan.less(arr[right], arr[i+1])) {
            swap(arr[(i+1)],arr[right]);
        }
        return i+1;
    }

    void do_quicksort(std::vector<smart_malloc> &arr, const int left, const int right) {
        if (left < right) {
            uint_fast64_t part = partition(arr, left, right);
            if (part > 0) do_quicksort(arr, left, part - 1);
            do_quicksort(arr, part + 1, right);
        }
    }

    void do_quicksort(std::vector<smart_malloc>& arr, const int max) {
        do_quicksort(arr, 0, arr.size()-1);
    }
};


#endif //VARSORTER_QUICKSORT_H

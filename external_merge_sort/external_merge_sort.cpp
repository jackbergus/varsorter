/*
 * external_merge_sort.cpp
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
// Created by giacomo on 08/05/19.
//

#include "external_merge_sort/external_merge_sort.h"

void_virtual_sorter::void_virtual_sorter() : virtual_sorter(false) {}

int void_virtual_sorter::compare(void *lhs, uint_fast64_t lhs_size, void *rhs, uint_fast64_t rhs_size) {
    return 0;
}


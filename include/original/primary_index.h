/*
 * index.h
 * This file is part of varsorter
 *
 * Copyright (C) 2017 - Giacomo Bergami
 *
 * Created on 10/08/17
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



#ifndef VARSORTER_INDEX_H
#define VARSORTER_INDEX_H

#include <cstdint>

/**
 * This data structure provides the indexing used within the storing function:
 */
struct primary_index {
    uint_fast64_t id;
    uint_fast64_t begin;
    uint_fast64_t end;

    primary_index();
    primary_index(uint_fast64_t begin, uint_fast64_t end);
};

/**
 * This data structure contains the secondary indexing data structure: this assumes that the first element has id 0...
 * This data structure allows to preserve the access in O(1) to all the elements that were provided
 */
struct secondary_index {
    uint_fast64_t id;
    uint_fast64_t offset_begin;

    secondary_index();
    secondary_index(uint_fast64_t id, uint_fast64_t begin);
};

#endif //VARSORTER_INDEX_H

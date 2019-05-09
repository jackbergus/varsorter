/*
 * ExternalULongKeyComparator.cpp
 * This file is part of varsorter
 *
 * Copyright (C) 2019 - Giacomo Bergami
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

//
// Created by giacomo on 08/05/19.
//

#include "ExternalULongKeyComparator.h"

#define get(ptr, size)  (*(((size) == sizeof(uint_fast64_t)*4) ? ((uint_fast64_t*)((char*)(ptr)+(sizeof(uint_fast64_t)*2))) : ((uint_fast64_t*)(ptr))))

bool ExternalULongKeyComparator::greaterThan(void *leftM, size_t leftS, void *rightM, size_t rightS) {
    return get(leftM, leftS) > get(rightM, rightS);
}

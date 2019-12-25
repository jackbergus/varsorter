/*
 * QuicksortComparatorTest.cpp
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

#include "external_merge_sort/QuicksortComparatorTest.h"

bool QuicksortComparatorTest::greaterThan(void *leftM, size_t leftS, void *rightM, size_t rightS) {
    std::string safeL{(const char *) leftM, leftS};
    std::string safeR{(const char *) rightM, rightS};
    long lI = std::stol(safeL.substr(1));
    long rI = std::stol(safeR.substr(1));
    return (lI > rI);
}

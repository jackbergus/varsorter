/*
 * QuicksortComparatorTest.h
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

#ifndef VARSORTER_QUICKSORTCOMPARATORTEST_H
#define VARSORTER_QUICKSORTCOMPARATORTEST_H


#include "QuicksortLessComparator.h"


class QuicksortComparatorTest : public QuicksortLessComparator {
public:
    bool greaterThan(void *leftM, size_t leftS, void *rightM, size_t rightS) override;
};


#endif //VARSORTER_QUICKSORTCOMPARATORTEST_H

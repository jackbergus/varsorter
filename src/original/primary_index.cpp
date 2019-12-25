/*
 * index.cpp
 * This file is part of varsorter
 *
 * Copyright (C) 2019 - Giacomo Bergami
 *
 * Created on 8/05/19
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

#include "original/primary_index.h"

primary_index::primary_index() : begin{0}, end{0} {}
primary_index::primary_index(uint_fast64_t begin, uint_fast64_t end) : begin(begin), end(end) {}


secondary_index::secondary_index() : id{0}, offset_begin{0} {}
secondary_index::secondary_index(uint_fast64_t id, uint_fast64_t begin) : id(id), offset_begin(begin) {}


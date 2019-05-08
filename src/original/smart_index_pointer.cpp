/*
 * smart_index_pointer.cpp
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

#include <cassert>
#include "smart_index_pointer.h"

smart_index_pointer::smart_index_pointer(index *externalPointer) : external_pointer(externalPointer) {
    fix_block_size = 0;
}

smart_index_pointer::smart_index_pointer(uint_fast64_t fixBlockSize) : fix_block_size(fixBlockSize) {
    external_pointer = nullptr;
    tmp.iov_len=fix_block_size;
}

struct index* smart_index_pointer::operator[](const uint_fast64_t &id) {
    if (external_pointer) {
        return &external_pointer[id];
    } else {
        this->unthread_safe.begin = id*fix_block_size;
        this->unthread_safe.end = (id+1)*fix_block_size;
        return &unthread_safe;
    }
}

smart_index_pointer::smart_index_pointer() {
    external_pointer = nullptr;
    fix_block_size = 1UL;
}

void smart_index_pointer::open(index *ptr) {
    fix_block_size = 0;
    external_pointer = ptr;
}

void smart_index_pointer::open(uint_fast64_t len) {
    external_pointer = nullptr;
    fix_block_size = len;
}

void smart_index_pointer::unsafe_swap(uint_fast64_t i, const uint_fast64_t &j, char *externalMemory) {
    assert(!external_pointer);
    assert(fix_block_size);
    tmp.iov_base = externalMemory + (i*fix_block_size);
    this->malloc.domalloc(fix_block_size);
    this->malloc.docopy(tmp); // safely copied
    memcpy(tmp.iov_base, externalMemory + (j*fix_block_size), fix_block_size);
    memcpy(externalMemory + (j*fix_block_size), this->malloc.malloced_iovec.iov_base, fix_block_size);
}

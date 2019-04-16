/*
 * smart_malloc.cpp
 * This file is part of varsorter
 *
 * Copyright (C) 2019 - Giacomo Bergami
 *
 * Created on 15/04/19
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

#include "smart_malloc.h"

void *smart_malloc::domalloc(size_t size) {
    if (malloced_iovec.iov_base == nullptr) {
        malloced_iovec.iov_base = malloc(size);
        malloced_iovec.iov_len = size;
    } else if (malloced_iovec.iov_len < size) {
        if (malloced_iovec.iov_base) {
            malloced_iovec.iov_base = realloc(malloced_iovec.iov_base, size);
            malloced_iovec.iov_len = size;
        }
    }
    return malloced_iovec.iov_base;
}

smart_malloc::~smart_malloc() {
    if (malloced_iovec.iov_len && malloced_iovec.iov_base) {
        free(malloced_iovec.iov_base);
    }
    malloced_iovec.iov_base = nullptr;
    malloced_iovec.iov_len = 0;
}

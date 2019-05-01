/*
 * inserter.cpp
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

 


#include "inserter.h"
#include "new_iovec.h"

extern "C" {
#include <unistd.h>
}

inserter::inserter() {
    toSerialize.begin = 0;
    toSerialize.end = 0;
    fdIndex = nullptr;
    fdKeyValueStorage = nullptr;
}

void inserter::open(std::string indexFile, std::string kvFile) {
    close();
    fdIndex = fopen(indexFile.c_str(), "a+");
    fdKeyValueStorage = fopen(kvFile.c_str(), "a+");
    index = indexFile;
    file = kvFile;
}

void inserter::close() {
    if (fdIndex) {
        fclose(fdIndex);
        fdIndex = 0;
    }
    if (fdKeyValueStorage) {
        fclose(fdKeyValueStorage);
        fdKeyValueStorage = 0;
    }
}

void inserter::writeKeyAndValue(void *mem, uint_fast64_t size) {
    risk_writeKeyAndValue_with_prev(mem,size,0);
}

void inserter::risk_writeKeyAndValue_with_prev(void *mem, uint_fast64_t mem_size, uint_fast64_t risk_prev_inserted_size) {
    fwrite(mem, mem_size, 1, fdKeyValueStorage);
    toSerialize.begin = toSerialize.end;
    toSerialize.end = toSerialize.begin + risk_prev_inserted_size + mem_size;
    fwrite(&toSerialize,sizeof(struct index),1,fdIndex);
}

inserter::~inserter() {
    if (fdIndex) {
        fclose(fdIndex);
        fdIndex = nullptr;
    }
    if (fdKeyValueStorage) {
        fclose(fdKeyValueStorage);
        fdKeyValueStorage = nullptr;
    }
    fdIndex = nullptr;
    fdKeyValueStorage = nullptr;
}

void inserter::risk_writeKeyAndValue_noindex(void *mem, uint_fast64_t size) {
    fwrite(mem, size, 1, fdKeyValueStorage);
}

void inserter::writeKeyAndValue(struct new_iovec &x) {
    writeKeyAndValue(x.iov_base, x.iov_len);
}

void inserter::writeKeyAndValue(struct iovec &x) {
    writeKeyAndValue(x.iov_base, x.iov_len);
}

void inserter::dounlink() {
    close();
    unlink(file.c_str());
    unlink(index.c_str());
}

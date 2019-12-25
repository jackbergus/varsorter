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

 


#include "original/inserter.h"
#include "original/new_iovec.h"

extern "C" {
#include <unistd.h>
}

inserter::inserter(bool secondary_use) : inserter{0, false, secondary_use} {}

inserter::inserter(uint_fast64_t fixed_size, bool secondary_use) : inserter{fixed_size, true, secondary_use} { }

inserter::inserter(uint_fast64_t fixed_size, bool isFixedSize, bool secondary_use) {
    toSerialize.begin = 0;
    toSerialize.end = 0;
    toSerialize.id = 0;
    toTmpSerialize.id = 0;
    toTmpSerialize.offset_begin = 0;
    useSecondaryIndex = secondary_use;
    fdPrimaryIndex = nullptr;
    fdSecondaryIndex = nullptr;
    fdKeyValueStorage = nullptr;
    hasFixedSizeInput = isFixedSize;
    this->fixed_size = fixed_size;
}

void inserter::open(std::string indexFile, std::string kvFile) {
    close();
    if (!hasFixedSizeInput) {
        fdPrimaryIndex = fopen(indexFile.c_str(), "a+");
        std::string secondaryFile = indexFile + "_secondary";
        if (useSecondaryIndex)
            fdSecondaryIndex = fopen(secondaryFile.c_str(), "a+");
    }
    fdKeyValueStorage = fopen(kvFile.c_str(), "a+");
    primary_index_file = indexFile;
    values_file = kvFile;
}

void inserter::open(uint_fast64_t fixed_size, std::string kvFile) {
    close();
    hasFixedSizeInput = true;
    fdPrimaryIndex = nullptr;
    fdSecondaryIndex = nullptr;
    this->fixed_size = fixed_size;
    fdKeyValueStorage = fopen(kvFile.c_str(), "a+");
    primary_index_file = "";
    values_file = kvFile;
}

void inserter::close() {
    if (fdPrimaryIndex) {
        fclose(fdPrimaryIndex);
        fdPrimaryIndex = nullptr;
    }
    if (fdSecondaryIndex) {
        fclose(fdSecondaryIndex);
        fdSecondaryIndex = nullptr;
    }
    if (fdKeyValueStorage) {
        fclose(fdKeyValueStorage);
        fdKeyValueStorage = nullptr;
    }
}

void inserter::writeKeyAndValue(void *mem, uint_fast64_t size, uint_fast64_t *secondary_ptr) {
    risk_writeKeyAndValue_with_prev(mem, size, 0, secondary_ptr);
}

void inserter::risk_writeKeyAndValue_with_prev(void *mem, uint_fast64_t mem_size, uint_fast64_t risk_prev_inserted_size,
                                               uint_fast64_t *secondary_ptr) {
    fwrite(mem, mem_size, 1, fdKeyValueStorage);
    fflush(fdKeyValueStorage);
    write_indexing_structures(mem_size, risk_prev_inserted_size, secondary_ptr);
}

void inserter::write_indexing_structures(uint_fast64_t mem_size, uint_fast64_t risk_prev_inserted_size,
                                         uint_fast64_t *id_ptr) {
    if (!hasFixedSizeInput) {
        toSerialize.id ++;
        toSerialize.begin = toSerialize.end;
        toSerialize.end = toSerialize.begin + risk_prev_inserted_size + mem_size;
        fwrite(&toSerialize, sizeof(struct primary_index), 1, fdPrimaryIndex);
        if (useSecondaryIndex) {
            id_ptr ? (toTmpSerialize.id = *id_ptr) : (toTmpSerialize.id++);
            toTmpSerialize.offset_begin = toSerialize.begin;
            fwrite(&toTmpSerialize, sizeof(struct secondary_index), 1, fdSecondaryIndex);
        }
    }
}

inserter::~inserter() {
    if (fdPrimaryIndex) {
        fclose(fdPrimaryIndex);
    }
    if (fdSecondaryIndex) {
        fclose(fdSecondaryIndex);
    }
    if (fdKeyValueStorage) {
        fclose(fdKeyValueStorage);
    }
    fdSecondaryIndex = nullptr;
    fdPrimaryIndex = nullptr;
    fdKeyValueStorage = nullptr;
}

void inserter::risk_writeKeyAndValue_noindex(void *mem, uint_fast64_t size) {
    fwrite(mem, size, 1, fdKeyValueStorage);
}

void inserter::writeKeyAndValue(struct new_iovec &x) {
    writeKeyAndValue(x.iov_base, x.iov_len, nullptr);
}

void inserter::writeKeyAndValue(struct iovec &x) {
    writeKeyAndValue(x.iov_base, x.iov_len, nullptr);
}

void inserter::dounlink() {
    close();
    unlink(values_file.c_str());
    if (!hasFixedSizeInput) {
        unlink(primary_index_file.c_str());
        if (useSecondaryIndex) {
            std::string sec = primary_index_file + "_secondary";
            unlink(sec.c_str());
        }
    }
}

bool inserter::usesSecondaryIndex() const {
    return useSecondaryIndex;
}

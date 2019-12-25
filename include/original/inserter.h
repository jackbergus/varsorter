/*
 * inserter.h
 * This file is part of inserter.h
 *
 * Copyright (C) 2017 - Giacomo Bergami
 * 
 * Created on 10/08/17
 *
 * inserter.h is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * inserter.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with inserter.h. If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef VARSORTER_INSERTER_H
#define VARSORTER_INSERTER_H


#include <cstdint>
#include <cstdio>
#include "primary_index.h"
#include <string>
#include <fstream>
#include <string>
#include <sstream>
#include <bits/types/struct_iovec.h>

class inserter {
    inserter(uint_fast64_t fixed_size, bool isFixedSize, bool secondary_use);

    struct primary_index toSerialize;
    std::string primary_index_file;

    struct secondary_index toTmpSerialize;
    std::string secondary_index_file;

    std::string values_file;
    bool hasFixedSizeInput;
    bool useSecondaryIndex;
    uint_fast64_t fixed_size;

public:
    FILE* fdPrimaryIndex;
    FILE* fdSecondaryIndex;
    FILE* fdKeyValueStorage;
    inserter(bool secondary_use);
    inserter(uint_fast64_t fixed_size, bool secondary_use);
    ~inserter();

    bool usesSecondaryIndex() const;

    /**
     * Opening the data structures underneath, if not already opened. This method is also fixed_size aware.
     *
     * @param indexFile     The file for the indexing data structure. This filename is ignored if the inserted has been
     *                      already initialised as a fixed_value element
     * @param kvFile        Always not ignored mapper to the file containing the actual values
     */
    void open(std::string indexFile, std::string kvFile);

    /**
     * This method forces to have a fixed size
     *
     * @param fixed_size
     * @param kvFile
     */
    void open(uint_fast64_t fixed_size, std::string kvFile);

    void close();
    void dounlink();

    void writeKeyAndValue(struct new_iovec& );
    void writeKeyAndValue(struct iovec& );
    void writeKeyAndValue(void *mem, uint_fast64_t size, uint_fast64_t *secondary_ptr);
    void risk_writeKeyAndValue_noindex(void* mem, uint_fast64_t size);
    void risk_writeKeyAndValue_with_prev(void *mem, uint_fast64_t mem_size, uint_fast64_t risk_prev_inserted_size,
                                         uint_fast64_t *secondary_ptr);

    /**
     * Serializing a primary memory object with a default serializer
     * @tparam T                        Type having a size_t serialize(FILE* ptr) method
     * @param obj                       Object with the default serializer
     * @param risk_prev_inserted_size   ???
     */
    template <typename T> size_t risk_writeKeyAndValue_with_prev(const T& obj, uint_fast64_t risk_prev_inserted_size) {
        size_t mem_size = obj.serialize(fdKeyValueStorage);
        fflush(fdKeyValueStorage);
        write_indexing_structures(mem_size, risk_prev_inserted_size, nullptr);
    }

private:
    void write_indexing_structures(uint_fast64_t mem_size, uint_fast64_t risk_prev_inserted_size,
                                   uint_fast64_t *id_ptr);
};


#endif //VARSORTER_INSERTER_H

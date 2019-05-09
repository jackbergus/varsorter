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
#include "index.h"
#include <string>
#include <fstream>
#include <string>
#include <sstream>
#include <bits/types/struct_iovec.h>

class inserter {
    inserter(uint_fast64_t fixed_size, bool isFixedSize);

    struct index toSerialize;
    std::string file, index;
    bool hasFixedSizeInput;
    uint_fast64_t fixed_size;

public:
    FILE* fdIndex;
    FILE* fdKeyValueStorage;
    inserter();
    inserter(uint_fast64_t fixed_size);
    ~inserter();

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
    void writeKeyAndValue(void* mem, uint_fast64_t size);
    void risk_writeKeyAndValue_noindex(void* mem, uint_fast64_t size);
    void risk_writeKeyAndValue_with_prev(void *mem, uint_fast64_t mem_size, uint_fast64_t risk_prev_inserted_size);
};


#endif //VARSORTER_INSERTER_H

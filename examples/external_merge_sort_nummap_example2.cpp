/*
 * external_merge_sort_nummap_example2.cpp
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

#include "../src/KeyValueStore.h"
#include "../external_merge_sort/ExternalULongPairComparator.h"

extern "C" {
#include <unistd.h>
}
#include <cstdlib>
#include <ctime>

int main(void) {
    std::string values{"keyvalue.bin"};
    std::string index{"index.bin"};
    unlink(index.c_str());
    unlink(values.c_str());
                                                // The data block now consists of the offsets to the elements, and then for the two values that we are serializing.
    KeyValueStore<ExternalULongPairComparator> c{16, values};

    //virtual_key_value_store c{index.c_str(), values.c_str()};
    // No. of Partitions of input file.
    int num_ways = 2;
    int run_size = 10;
    int size = num_ways*run_size;
    srand(time(NULL));

    for (int i = 0; i < size; i++) {
        uint_fast64_t arr[2];
        arr[0] = rand();
        arr[1] = rand();
        c.insert(&arr, 16);
        std::cout << arr[0] << ":" << arr[1] << std::endl;
    }
    std::cout << "Closing file & sorting" << std::endl;
    c.c.close();
    external_merge_sort<ExternalULongPairComparator> ems{16};
    ems.run(values, index, num_ways);
    std::cout << "sorting finished" << std::endl;
    c.sorter = new void_virtual_sorter();
    c.openIfRequired(16, values);
    for (virtual_sorter::iterator it = c.begin(), end = c.end(); it != end; it++ ) {
        std::cout << ((uint_fast64_t*)it->iov_base)[0] << ":" << ((uint_fast64_t*)it->iov_base)[1] << std::endl;
    }

}

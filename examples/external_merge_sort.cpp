/*
 * main.cpp
 * This file is part of varsorter
 *
 * Copyright (C) 2019 - Giacomo Bergami
 *
 *  varsorter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *  varsorter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with  varsorter. If not, see <http://www.gnu.org/licenses/>.
 */

 
extern "C" {
#include <unistd.h>
}
#include <iostream>
#include <sstream>
#include "examples.h"
#include "../src/original/java_utils.h"
#include "../src/original/serializer_with_sort.h"
#include "../src/original/smart_malloc.h"
#include "../external_merge_sort/external_merge_sort.h"
#include "../external_merge_sort/QuicksortComparatorTest.h"


/**
 *
 * @param c         serializer
 * @param x         string to be written
 */
#define write_string2(c,x)  c.risk_insert((void*)x.c_str(),x.length())

/**
 * The only purpose of this main is to run some examples, and to show that the current code works :)
 * @return
 */
int main() {
/*
    std::cerr << "first example (sleep 3)" ;
    sleep(3);
    std::cerr << "..." << std::endl;
    // This provides an example of how to serialize values without explicit indexing data structures (key)
    // These will be only be provided for traversing the mutable data.
    serialze_with_sort_example();

    std::cerr << "second example (sleep 3)" ;
    sleep(3);
    std::cerr << "..." << std::endl;
    string_key_store_example();

    std::cerr << "Bye!" << std::endl;
    return 0;*/

     /*   IntComparator test = intcmp;
        int l = -100, r = 20;
        int res = test(&l, &r);
        std::cout<< res << std::endl;
*/

    {
        // File that will contain the values
        std::string file1 = "valori_di_space_int.txt";
        // Index where the offsets to the strings will be inserted
        std::string file2 = "index.bin";
        // Removing the previously existing files
        unlink(file2.c_str());
        unlink(file1.c_str());
        // Opening the file
        serializer_with_sort c{file2, file1};
        // No. of Partitions of input file.
        int num_ways = 2;
        // The size of each partition
        int run_size = 10;
        srand(time(NULL));
        // generate input
        for (int i = 0; i < num_ways * run_size; i++) {
            std::string toWrite = " "+std::to_string(rand());
            write_string2(c, toWrite);
        }

        c.c.close();
        external_merge_sort<QuicksortComparatorTest> ems;
        ems.run(file1, file2, num_ways);
    }


}

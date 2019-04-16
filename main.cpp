/*
 * main.cpp
 * This file is part of varsorter
 *
 * Copyright (C) 2017 - Giacomo Bergami
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
#include "examples/examples.h"

/**
 * The only purpose of this main is to run some examples, and to show that the current code works :)
 * @return
 */
int main() {

    std::cerr << "first example (sleep 3)" ;
    sleep(3);
    std::cerr << "..." << std::endl;
    // This provides an example of how to serialize values without explicit indexing data structures (key)
    // These will be only be provided for traversing the mutable data.
    //serialze_with_sort_example();

    std::cerr << "second example (sleep 3)" ;
    sleep(3);
    std::cerr << "..." << std::endl;
    string_key_store_example();

    std::cerr << "Bye!" << std::endl;
    return 0;

}

/*
 * serialize_with_sort_example.cpp
 * This file is part of varsorter
 *
 * Copyright (C) 2017 - Giacomo Bergami
 * 
 * Created on 11/08/17
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

 

#include "original/virtual_sorter.h"
#include "original/serializer_with_sort.h"

class string_sorter : public virtual_sorter {
public:
    string_sorter() : virtual_sorter(false) {}

    int compare(void *lhs, uint_fast64_t lhs_size, void *rhs, uint_fast64_t rhs_size) {
        int toret = strncmp(static_cast<const char *>(lhs), static_cast<const char *>(rhs), lhs_size < rhs_size ? lhs_size : rhs_size);
        if (lhs_size == rhs_size)
            return toret;
        else if (toret == 0) {
            return lhs_size < rhs_size ? -1 : 1;
        } else return toret;
    }

};

/**
 *
 * @param c         serializer
 * @param x         string to be written
 */
#define write_string2(c,x)  c.risk_insert((void*)x.c_str(),x.length(), nullptr)

int main() {
    // File that will contain the values
    std::string file1 = "valori_di_stringa.txt";
    // Index where the offsets to the strings will be inserted
    std::string file2 = "index.bin";

    // Removing the previously existing files
    unlink(file2.c_str());
    unlink(file1.c_str());

    // Opening the file
    serializer_with_sort c{file2, file1, false};

    // Associating a sorter for sorting the values
    c.sorter = new string_sorter();

    // Writing the data
    {
        std::string toWrite = "benissimo";
        write_string2(c, toWrite);

        toWrite = "viva";
        write_string2(c, toWrite);

        toWrite = "la";
        write_string2(c, toWrite);

        toWrite = "foca";
        write_string2(c, toWrite);

        toWrite = "e";
        write_string2(c, toWrite);

        toWrite = "chi";
        write_string2(c, toWrite);

        toWrite = "la";
        write_string2(c, toWrite);

        toWrite = "pesca";
        write_string2(c, toWrite);
    }

    // Iterating over the set of already-serialized elements, without necessairly odering them
    virtual_sorter::iterator it;
    char elems[200];
    for (it = c.begin(); it != c.end(); it++ ) {
        memset(elems,0,200);
        strncpy(elems, static_cast<const char *>(it->iov_base), it->iov_len);
        std::cout << elems << std::endl;
    }

    // Preprocessing: now the elements are ordered
    // Please note that this is done automatically when we're searching for an element.
    // Therefore, if I add some data and then I look for another element, this will constantly search for the data.
    //
    // As you can see, RocksDB is more flexible but, in cases that you don't need to search for the data while you're
    // inserting that, the actual key-value store implementation might be far way more efficient than RocksDB.
    c.sortElement();

    // Printing the elments, all sorted
    std::cout << "---" << std::endl;
    for (it = c.begin(); it != c.end(); it++ ) {
        memset(elems,0,200);
        strncpy(elems, static_cast<const char *>(it->iov_base), it->iov_len);
        std::cout << elems << std::endl;
    }
    std::cout << "---" << std::endl;

    // Trying to find a find a stirng that doesnt exist
    std::string tU{"ecceziunale"};
    if ((it = c.searchFor((void*)tU.c_str(), tU.length())) == c.end()) {
        std::cout << "[[Element not found]]" << std::endl;
    }

    // Trying to find a string that exists
    std::string tW{"e"};
    if ((it = c.searchFor((void*)tW.c_str(), 1)) != c.end()) {
        std::cout << "[[Element found]]" << std::endl;
    }

    // Updates the it iterator with the new value
    c.update(it, (void*)tU.c_str(), tU.length());

    // Re-scanning the list in order to check if the element has been append to the end
    for (it = c.begin(); it != c.end(); it++ ) {
        memset(elems,0,200);
        strncpy(elems, static_cast<const char *>(it->iov_base), it->iov_len);
        std::cout << elems << std::endl;
    }
}

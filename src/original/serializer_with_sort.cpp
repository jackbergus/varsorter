/*
 * serializer_with_sort.cpp
 * This file is part of varsorter
 *
 * Copyright (C) 2019 - Giacomo Bergami
 *
 * Created on 10/08/17
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



#include <cassert>
#include "original/serializer_with_sort.h"

serializer_with_sort::serializer_with_sort(std::string indexFile, std::string valuesFile, bool use_secondary)
        : index_path{indexFile}, values_path{valuesFile}, c{use_secondary} {
    fixed_size = 0L;
    isFixedSize_sws = false;
}

serializer_with_sort::serializer_with_sort(uint_fast64_t fixed_size, std::string valuesFile, bool use_secondary)
        : fixed_size{fixed_size}, values_path{valuesFile},
          c{fixed_size, use_secondary} {
    isFixedSize_sws = true;
}

bool serializer_with_sort::risk_insert(void *buff, uint_fast64_t len, uint_fast64_t *secondary_ptr) {
    if (buff != nullptr && len != 0) {
        if (hasSorted) {
            sorter->doclose();
            hasSorted = false;
        }
        if (hasInserted) {
            if (!hasRiskInsert) {
                sorter->doclose();
                c.open(index_path, values_path); // fixed_size aware
            }
        } else {
            c.open(index_path, values_path);  // fixed_size aware
        }
        c.writeKeyAndValue(buff, len, secondary_ptr);
        hasInserted = true;
        hasRiskInsert = true;
        return true;
    }
    return false;
}

void serializer_with_sort::sortElement() {
    if (!hasSorted) {
        c.close();
        if (sorter)
            sorter->doclose();
        hasInserted = false;
        hasRiskInsert = false;
        if (isFixedSize_sws)
            sorter->openvirtual_sorter(fixed_size, values_path);
        else
            sorter->openvirtual_sorter(index_path, values_path);
        sorter->sortPair();
        hasSorted = true;
    }
}

int serializer_with_sort::update(void *old, uint_fast64_t oldLen, void *neu, uint_fast64_t newLen,
                                 uint_fast64_t *secondary_ptr) {
    if (old == nullptr || oldLen == 0) {
        return risk_insert(neu, newLen, secondary_ptr) ? 0 : -1;
    } else {
        virtual_sorter::iterator ptr = searchFor(old, oldLen); // c is closed
        const virtual_sorter::iterator &end_n = end();
        // If the pointer points to the end (the old element is not there)
        if (ptr == end_n) {
            // Then I just insert the new value
            return risk_insert(neu, newLen, secondary_ptr) ? 0 : -1;
        }

        // updates the pointer
        std::cerr << "WARNING: updating the value without changing the id" << std::endl;
        return update_internal(ptr, neu, newLen, secondary_ptr);
    }
}

virtual_sorter::iterator serializer_with_sort::begin() {
    if (hasRiskInsert) {
        c.close();
        if (sorter)
            sorter->doclose();
        if (sorter) {
            if (isFixedSize_sws)
                sorter->openvirtual_sorter(fixed_size, values_path);
            else
                sorter->openIfRequired(index_path, values_path);
        }
    }
    return sorter->begin(isFixedSize_sws);
}

virtual_sorter::iterator serializer_with_sort::end() {
    if (hasRiskInsert) {
        c.close();
        if (sorter)
            sorter->doclose();
        if (sorter) {
            if (isFixedSize_sws)
                sorter->openvirtual_sorter(fixed_size, values_path);
            else
                sorter->openIfRequired(index_path, values_path);
        }
    }
    return sorter->end(isFixedSize_sws);
}

int serializer_with_sort::update(virtual_sorter::iterator &iterator, void *pVoid, unsigned long i) {
    return iterator != end() ? update_internal(iterator, pVoid, i, nullptr) : -1;
}

int serializer_with_sort::update_internal(virtual_sorter::iterator &ptr, void *neu, unsigned long newLen,
                                          uint_fast64_t *secondary_ptr) {
    hasInserted = true;
    hasRiskInsert = true;
    hasSorted = false; // If I now need to further search, the data is no more sorted

    // If the new value is shorter than the previous one, then I can overwrite the old information with this new one.
    if ((isFixedSize_sws) || (ptr->iov_len >= newLen)) {
        if (!isFixedSize_sws) ptr.setNewLen(newLen);
        sorter->risk_overwrite_as_memcpy(ptr->iov_base, neu, newLen); // c is closed
        // Do nothing else, and return.
        return 1;
    }

    // update the index definition with the pointer to thne new data
    ptr.updateWith(sorter->mmap_index_File[sorter->struct_primary_index_size / sizeof(struct primary_index) - 1].end,
                   newLen);
    if (secondary_ptr && sorter->usesSecondaryIndex() && sorter->mmap_2index_File)  {
        assert(sorter->mmap_2index_File[*secondary_ptr].id == *secondary_ptr);
        sorter->mmap_2index_File[*secondary_ptr].offset_begin = sorter->mmap_index_File[sorter->struct_primary_index_size / sizeof(struct primary_index) - 1].end;
    }

    // I must close the sorted, because it must be reopened only when required
    sorter->doclose();
    // Opens both the index and the values in append mode
    c.open(index_path, values_path);  // fixed_size aware
    // Add the data at the end of the file (that is, after opening that in append mode).
    c.risk_writeKeyAndValue_noindex(neu, newLen);
    return 2;
}

virtual_sorter::iterator serializer_with_sort::searchFor(void *buff, uint_fast64_t len) {
    if (hasRiskInsert) {
        c.close();
        if (sorter)
            sorter->doclose();
        if (isFixedSize_sws)
            sorter->openvirtual_sorter(fixed_size, values_path);
        else
            sorter->openIfRequired(index_path, values_path);
    }
    if (buff == nullptr || len == 0) {
        return sorter->end();
    } else {
        return sorter->binSearch(buff, len);
    }
}

serializer_with_sort::~serializer_with_sort() {
    c.close();
    if (sorter)
        sorter->doclose();
    sorter = nullptr;
}

bool serializer_with_sort::iovec_multiinsert(std::initializer_list<struct iovec> list) {
    uint_fast64_t size = 0;
    if (hasSorted) {
        sorter->doclose();
        hasSorted = false;
    }
    if (hasInserted) {
        if (!hasRiskInsert) {
            sorter->doclose();
            c.open(index_path, values_path);  // fixed_size aware
        }
    } else {
        c.open(index_path, values_path);  // fixed_size aware
    }
    unsigned long lsize = list.size(), i = 0;
    for (auto elem : list) {
        if (i < lsize) {
            if (size + elem.iov_len > 0 && elem.iov_base) {
                c.risk_writeKeyAndValue_noindex(elem.iov_base, elem.iov_len);
                hasInserted = true;
                hasRiskInsert = true;
                size += size;
            }
        } else {
            if (size + elem.iov_len > 0 && elem.iov_base) {
                c.risk_writeKeyAndValue_with_prev(elem.iov_base, elem.iov_len, size, nullptr);
                hasInserted = true;
                hasRiskInsert = true;
            }
        }
        i++;
    }
    return hasInserted && hasRiskInsert;
}

bool serializer_with_sort::risk_insert(struct iovec &ptr, uint_fast64_t *secondary_ptr) {
    return risk_insert(ptr.iov_base, ptr.iov_len, secondary_ptr);
}

int serializer_with_sort::insert(struct new_iovec &x, uint_fast64_t *secondary_ptr) {
    return insert(x.iov_base, x.iov_len, secondary_ptr);
}

int serializer_with_sort::insert(void *ptr, uint_fast64_t len, uint_fast64_t *secondary_ptr) {
    return update(nullptr, 0, ptr, len, secondary_ptr);
}

void serializer_with_sort::unlink() {
    c.dounlink();
    if (sorter)
        sorter->doclose();
    sorter = nullptr;
}


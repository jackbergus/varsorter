cmake_minimum_required(VERSION 3.7)
project(varsorter)

set(CMAKE_CXX_STANDARD 17)

ADD_LIBRARY(libvarsorter STATIC

            src/original/mmapFile.cpp
            src/original/mmapFile.h
            src/original/inserter.cpp
            src/original/inserter.h
            src/original/index.h
            src/original/virtual_sorter.cpp
            src/original/virtual_sorter.h
            src/original/serializer_with_sort.cpp
            src/original/serializer_with_sort.h
            src/original/virtual_key_value_store.cpp
            src/original/virtual_key_value_store.h
            src/original/smart_malloc.cpp
            src/original/smart_malloc.h
            src/original/new_iovec.cpp
            src/original/new_iovec.h
            src/original/java_utils.h
            external_merge_sort/external_merge_sort.h
            external_merge_sort/quicksort.h
            external_merge_sort/QuicksortLessComparator.h
            external_merge_sort/QuicksortLessComparatorKeyValue.h
            external_merge_sort/QuicksortComparatorTest.cpp
            external_merge_sort/QuicksortComparatorTest.h
        src/KeyValueStore.h
        )

set(EXTERNAL_MERGE_SORT_FILES   )

add_executable(serialize_with_sort_example
               examples/serialize_with_sort_example.cpp )

add_executable(string_key_store_example
               examples/string_key_store_example.cpp)

add_executable(external_sorter_example
               examples/external_merge_sort.cpp)

TARGET_LINK_LIBRARIES(serialize_with_sort_example libvarsorter )
TARGET_LINK_LIBRARIES(string_key_store_example libvarsorter )
TARGET_LINK_LIBRARIES(external_sorter_example libvarsorter )
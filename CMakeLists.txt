cmake_minimum_required(VERSION 3.7)
project(varsorter)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)

ADD_LIBRARY(libvarsorter STATIC

            src/original/mmapFile.cpp
        include/original/mmapFile.h
            src/original/inserter.cpp
        include/original/inserter.h
        include/original/primary_index.h
            src/original/virtual_sorter.cpp
        include/original/virtual_sorter.h
            src/original/serializer_with_sort.cpp
        include/original/serializer_with_sort.h
            src/original/virtual_key_value_store.cpp
        include/original/virtual_key_value_store.h
            src/original/smart_malloc.cpp
        include/original/smart_malloc.h
            src/original/new_iovec.cpp
        include/original/new_iovec.h
        include/original/java_utils.h
            src/original/smart_index_pointer.cpp
        include/original/smart_index_pointer.h
        src/original/primary_index.cpp
        include/external_merge_sort/quicksort.h
        include/external_merge_sort/QuicksortLessComparator.h
        include/external_merge_sort/QuicksortLessComparatorKeyValue.h
            external_merge_sort/QuicksortComparatorTest.cpp
        include/external_merge_sort/QuicksortComparatorTest.h
            external_merge_sort/ExternalULongKeyComparator.cpp
        include/external_merge_sort/ExternalULongKeyComparator.h
        external_merge_sort/ExternalULongPairComparator.cpp
        include/external_merge_sort/ExternalULongPairComparator.h
            external_merge_sort/external_merge_sort.cpp
        include/KeyValueStore.h)

set(EXTERNAL_MERGE_SORT_FILES   )

add_executable(serialize_with_sort_example
               examples/serialize_with_sort_example.cpp )

add_executable(string_key_store_example
               examples/string_key_store_example.cpp)

add_executable(external_sorter_example
        examples/external_merge_sort_example.cpp)

add_executable(external_merge_sort_nummap
        examples/external_merge_sort_nummap_example.cpp)

add_executable(external_merge_sort_nummap2
        examples/external_merge_sort_nummap_example2.cpp)

TARGET_LINK_LIBRARIES(serialize_with_sort_example libvarsorter )
TARGET_LINK_LIBRARIES(string_key_store_example libvarsorter )
TARGET_LINK_LIBRARIES(external_sorter_example libvarsorter )
TARGET_LINK_LIBRARIES(external_merge_sort_nummap libvarsorter )
TARGET_LINK_LIBRARIES(external_merge_sort_nummap2 libvarsorter )
cmake_minimum_required(VERSION 3.7)
project(varsorter)

set(CMAKE_CXX_STANDARD 17)

ADD_LIBRARY(libvarsorter STATIC

            src/original/inserter.cpp
            src/original/inserter.h
        src/original/virtual_sorter.cpp
            src/original/virtual_sorter.h
            src/original/serializer_with_sort.cpp
            src/original/serializer_with_sort.h
            src/original/virtual_key_value_store.cpp
            src/original/virtual_key_value_store.h
            src/original/java_utils.h
        external_merge_sort/quicksort.h
            external_merge_sort/QuicksortLessComparator.h
            external_merge_sort/QuicksortLessComparatorKeyValue.h
            external_merge_sort/QuicksortComparatorTest.cpp
            external_merge_sort/QuicksortComparatorTest.h
            external_merge_sort/ExternalULongKeyComparator.cpp
            external_merge_sort/ExternalULongKeyComparator.h
        external_merge_sort/ExternalULongPairComparator.cpp
        external_merge_sort/ExternalULongPairComparator.h
            external_merge_sort/external_merge_sort.cpp
            src/KeyValueStore.h )
target_link_libraries(libvarsorter yaucl_memory yaucl_fs)

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

TARGET_LINK_LIBRARIES(serialize_with_sort_example yaucl_memory yaucl_fs libvarsorter )
TARGET_LINK_LIBRARIES(string_key_store_example yaucl_memory yaucl_fs libvarsorter )
TARGET_LINK_LIBRARIES(external_sorter_example yaucl_memory yaucl_fs libvarsorter )
TARGET_LINK_LIBRARIES(external_merge_sort_nummap yaucl_memory yaucl_fs  libvarsorter )
TARGET_LINK_LIBRARIES(external_merge_sort_nummap2 yaucl_memory yaucl_fs libvarsorter )
cmake_minimum_required(VERSION 3.7)
project(varsorter)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp src/mmapFile.cpp src/mmapFile.h src/inserter.cpp src/inserter.h src/index.h src/virtual_sorter.cpp src/virtual_sorter.h src/serializer_with_sort.cpp src/serializer_with_sort.h examples/serialize_with_sort_example.cpp src/virtual_key_value_store.cpp src/virtual_key_value_store.h examples/string_key_store_example.cpp  examples/string_key_store_example.cpp examples/examples.h)
add_executable(varsorter ${SOURCE_FILES})
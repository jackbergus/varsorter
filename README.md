# Varsorter
~ Giacomo Bergami (c) 2017 -- 2019

## Why another library for external sorting the data? (& caveats)

This library was ment to overcome two of the major problems in current C++ libraries:
* Sorting (n log(n)) after bulk insertion (+n) inserting is more efficient than performing an insertion sort on trees (∑log(i) from i=1 to n)
    * If you don't need more advanced features like the ones proposed by RocksDB (data updates), this other solution is far way more efficient and straightforward.
    * Please remember that relational databases overcome to that problem by just appending the data at the end of the file, and then re-sorting that again. That is why the current implementation opens the file in append/read mode, so that new-data could be later on re-sorted after doing some insertions.
    * This is a manually tested observation, I did not have time to do any official "benchmark" to justify this bold statement.
    
* Big data libraries like STXXL are only designed for data with fixed value size while, on the other hand, their size could vary
    * For this reason, we can store collections of flexible data length with additional offset indices.
    * In this way, you could use this library both for sorting data with variadic size and for dealing with key-value stores.

* External dependencies may take the compiling and linking process quite time consuming
    * For this reason, this library is ''minimal'', as it  doesn't require third part dependencies.

This implementation is quite straightforward, and can be refactored and made more efficient as it actually is right now.

## Dependencies

This library requires [yaucl](https://github.com/datagramdb/yaucl): please install it before compiling the project.

## TODO (?) List:
1. Some additional unit testing to provide the correctness of the implementation should be provided, too.

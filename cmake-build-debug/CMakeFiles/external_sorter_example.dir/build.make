# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/external_sorter_example.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/external_sorter_example.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/external_sorter_example.dir/flags.make

CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.o: CMakeFiles/external_sorter_example.dir/flags.make
CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.o: ../examples/external_merge_sort_example.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.o -c /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter/examples/external_merge_sort_example.cpp

CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter/examples/external_merge_sort_example.cpp > CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.i

CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter/examples/external_merge_sort_example.cpp -o CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.s

# Object files for target external_sorter_example
external_sorter_example_OBJECTS = \
"CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.o"

# External object files for target external_sorter_example
external_sorter_example_EXTERNAL_OBJECTS =

external_sorter_example: CMakeFiles/external_sorter_example.dir/examples/external_merge_sort_example.cpp.o
external_sorter_example: CMakeFiles/external_sorter_example.dir/build.make
external_sorter_example: liblibvarsorter.a
external_sorter_example: CMakeFiles/external_sorter_example.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable external_sorter_example"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/external_sorter_example.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/external_sorter_example.dir/build: external_sorter_example

.PHONY : CMakeFiles/external_sorter_example.dir/build

CMakeFiles/external_sorter_example.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/external_sorter_example.dir/cmake_clean.cmake
.PHONY : CMakeFiles/external_sorter_example.dir/clean

CMakeFiles/external_sorter_example.dir/depend:
	cd /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter/cmake-build-debug /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter/cmake-build-debug /media/giacomo/Data/elementi/submodules/fuzzyStringMatching/third_party/varsorter/cmake-build-debug/CMakeFiles/external_sorter_example.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/external_sorter_example.dir/depend

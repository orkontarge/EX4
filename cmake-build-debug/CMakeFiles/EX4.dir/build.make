# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/avigayil/Documents/Clion/clion-2021.3.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/avigayil/Documents/Clion/clion-2021.3.3/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/avigayil/Documents/OS/EX4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/avigayil/Documents/OS/EX4/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/EX4.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/EX4.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/EX4.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/EX4.dir/flags.make

CMakeFiles/EX4.dir/SimpleTest.cpp.o: CMakeFiles/EX4.dir/flags.make
CMakeFiles/EX4.dir/SimpleTest.cpp.o: ../SimpleTest.cpp
CMakeFiles/EX4.dir/SimpleTest.cpp.o: CMakeFiles/EX4.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/avigayil/Documents/OS/EX4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/EX4.dir/SimpleTest.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/EX4.dir/SimpleTest.cpp.o -MF CMakeFiles/EX4.dir/SimpleTest.cpp.o.d -o CMakeFiles/EX4.dir/SimpleTest.cpp.o -c /home/avigayil/Documents/OS/EX4/SimpleTest.cpp

CMakeFiles/EX4.dir/SimpleTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/EX4.dir/SimpleTest.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/avigayil/Documents/OS/EX4/SimpleTest.cpp > CMakeFiles/EX4.dir/SimpleTest.cpp.i

CMakeFiles/EX4.dir/SimpleTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/EX4.dir/SimpleTest.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/avigayil/Documents/OS/EX4/SimpleTest.cpp -o CMakeFiles/EX4.dir/SimpleTest.cpp.s

CMakeFiles/EX4.dir/PhysicalMemory.cpp.o: CMakeFiles/EX4.dir/flags.make
CMakeFiles/EX4.dir/PhysicalMemory.cpp.o: ../PhysicalMemory.cpp
CMakeFiles/EX4.dir/PhysicalMemory.cpp.o: CMakeFiles/EX4.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/avigayil/Documents/OS/EX4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/EX4.dir/PhysicalMemory.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/EX4.dir/PhysicalMemory.cpp.o -MF CMakeFiles/EX4.dir/PhysicalMemory.cpp.o.d -o CMakeFiles/EX4.dir/PhysicalMemory.cpp.o -c /home/avigayil/Documents/OS/EX4/PhysicalMemory.cpp

CMakeFiles/EX4.dir/PhysicalMemory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/EX4.dir/PhysicalMemory.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/avigayil/Documents/OS/EX4/PhysicalMemory.cpp > CMakeFiles/EX4.dir/PhysicalMemory.cpp.i

CMakeFiles/EX4.dir/PhysicalMemory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/EX4.dir/PhysicalMemory.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/avigayil/Documents/OS/EX4/PhysicalMemory.cpp -o CMakeFiles/EX4.dir/PhysicalMemory.cpp.s

CMakeFiles/EX4.dir/VirtualMemory.cpp.o: CMakeFiles/EX4.dir/flags.make
CMakeFiles/EX4.dir/VirtualMemory.cpp.o: ../VirtualMemory.cpp
CMakeFiles/EX4.dir/VirtualMemory.cpp.o: CMakeFiles/EX4.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/avigayil/Documents/OS/EX4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/EX4.dir/VirtualMemory.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/EX4.dir/VirtualMemory.cpp.o -MF CMakeFiles/EX4.dir/VirtualMemory.cpp.o.d -o CMakeFiles/EX4.dir/VirtualMemory.cpp.o -c /home/avigayil/Documents/OS/EX4/VirtualMemory.cpp

CMakeFiles/EX4.dir/VirtualMemory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/EX4.dir/VirtualMemory.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/avigayil/Documents/OS/EX4/VirtualMemory.cpp > CMakeFiles/EX4.dir/VirtualMemory.cpp.i

CMakeFiles/EX4.dir/VirtualMemory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/EX4.dir/VirtualMemory.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/avigayil/Documents/OS/EX4/VirtualMemory.cpp -o CMakeFiles/EX4.dir/VirtualMemory.cpp.s

# Object files for target EX4
EX4_OBJECTS = \
"CMakeFiles/EX4.dir/SimpleTest.cpp.o" \
"CMakeFiles/EX4.dir/PhysicalMemory.cpp.o" \
"CMakeFiles/EX4.dir/VirtualMemory.cpp.o"

# External object files for target EX4
EX4_EXTERNAL_OBJECTS =

EX4: CMakeFiles/EX4.dir/SimpleTest.cpp.o
EX4: CMakeFiles/EX4.dir/PhysicalMemory.cpp.o
EX4: CMakeFiles/EX4.dir/VirtualMemory.cpp.o
EX4: CMakeFiles/EX4.dir/build.make
EX4: CMakeFiles/EX4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/avigayil/Documents/OS/EX4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable EX4"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/EX4.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/EX4.dir/build: EX4
.PHONY : CMakeFiles/EX4.dir/build

CMakeFiles/EX4.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/EX4.dir/cmake_clean.cmake
.PHONY : CMakeFiles/EX4.dir/clean

CMakeFiles/EX4.dir/depend:
	cd /home/avigayil/Documents/OS/EX4/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/avigayil/Documents/OS/EX4 /home/avigayil/Documents/OS/EX4 /home/avigayil/Documents/OS/EX4/cmake-build-debug /home/avigayil/Documents/OS/EX4/cmake-build-debug /home/avigayil/Documents/OS/EX4/cmake-build-debug/CMakeFiles/EX4.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/EX4.dir/depend


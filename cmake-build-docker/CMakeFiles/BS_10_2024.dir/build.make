# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /tmp/BS_10_2024

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/BS_10_2024/cmake-build-docker

# Include any dependencies generated for this target.
include CMakeFiles/BS_10_2024.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/BS_10_2024.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/BS_10_2024.dir/flags.make

CMakeFiles/BS_10_2024.dir/main.c.o: CMakeFiles/BS_10_2024.dir/flags.make
CMakeFiles/BS_10_2024.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/BS_10_2024/cmake-build-docker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/BS_10_2024.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/BS_10_2024.dir/main.c.o   -c /tmp/BS_10_2024/main.c

CMakeFiles/BS_10_2024.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/BS_10_2024.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/BS_10_2024/main.c > CMakeFiles/BS_10_2024.dir/main.c.i

CMakeFiles/BS_10_2024.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/BS_10_2024.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/BS_10_2024/main.c -o CMakeFiles/BS_10_2024.dir/main.c.s

CMakeFiles/BS_10_2024.dir/main.c.o.requires:

.PHONY : CMakeFiles/BS_10_2024.dir/main.c.o.requires

CMakeFiles/BS_10_2024.dir/main.c.o.provides: CMakeFiles/BS_10_2024.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/BS_10_2024.dir/build.make CMakeFiles/BS_10_2024.dir/main.c.o.provides.build
.PHONY : CMakeFiles/BS_10_2024.dir/main.c.o.provides

CMakeFiles/BS_10_2024.dir/main.c.o.provides.build: CMakeFiles/BS_10_2024.dir/main.c.o


# Object files for target BS_10_2024
BS_10_2024_OBJECTS = \
"CMakeFiles/BS_10_2024.dir/main.c.o"

# External object files for target BS_10_2024
BS_10_2024_EXTERNAL_OBJECTS =

BS_10_2024: CMakeFiles/BS_10_2024.dir/main.c.o
BS_10_2024: CMakeFiles/BS_10_2024.dir/build.make
BS_10_2024: CMakeFiles/BS_10_2024.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/BS_10_2024/cmake-build-docker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable BS_10_2024"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/BS_10_2024.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/BS_10_2024.dir/build: BS_10_2024

.PHONY : CMakeFiles/BS_10_2024.dir/build

CMakeFiles/BS_10_2024.dir/requires: CMakeFiles/BS_10_2024.dir/main.c.o.requires

.PHONY : CMakeFiles/BS_10_2024.dir/requires

CMakeFiles/BS_10_2024.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/BS_10_2024.dir/cmake_clean.cmake
.PHONY : CMakeFiles/BS_10_2024.dir/clean

CMakeFiles/BS_10_2024.dir/depend:
	cd /tmp/BS_10_2024/cmake-build-docker && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/BS_10_2024 /tmp/BS_10_2024 /tmp/BS_10_2024/cmake-build-docker /tmp/BS_10_2024/cmake-build-docker /tmp/BS_10_2024/cmake-build-docker/CMakeFiles/BS_10_2024.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/BS_10_2024.dir/depend


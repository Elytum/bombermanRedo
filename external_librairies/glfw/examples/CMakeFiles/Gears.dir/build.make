# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.3

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
CMAKE_COMMAND = /nfs/zfs-student-3/users/2014/achazal/.brew/Cellar/cmake/3.3.1/bin/cmake

# The command to remove a file.
RM = /nfs/zfs-student-3/users/2014/achazal/.brew/Cellar/cmake/3.3.1/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw

# Include any dependencies generated for this target.
include examples/CMakeFiles/Gears.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/Gears.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/Gears.dir/flags.make

examples/CMakeFiles/Gears.dir/gears.c.o: examples/CMakeFiles/Gears.dir/flags.make
examples/CMakeFiles/Gears.dir/gears.c.o: examples/gears.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object examples/CMakeFiles/Gears.dir/gears.c.o"
	cd /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/Gears.dir/gears.c.o   -c /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples/gears.c

examples/CMakeFiles/Gears.dir/gears.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Gears.dir/gears.c.i"
	cd /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples/gears.c > CMakeFiles/Gears.dir/gears.c.i

examples/CMakeFiles/Gears.dir/gears.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Gears.dir/gears.c.s"
	cd /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples/gears.c -o CMakeFiles/Gears.dir/gears.c.s

examples/CMakeFiles/Gears.dir/gears.c.o.requires:

.PHONY : examples/CMakeFiles/Gears.dir/gears.c.o.requires

examples/CMakeFiles/Gears.dir/gears.c.o.provides: examples/CMakeFiles/Gears.dir/gears.c.o.requires
	$(MAKE) -f examples/CMakeFiles/Gears.dir/build.make examples/CMakeFiles/Gears.dir/gears.c.o.provides.build
.PHONY : examples/CMakeFiles/Gears.dir/gears.c.o.provides

examples/CMakeFiles/Gears.dir/gears.c.o.provides.build: examples/CMakeFiles/Gears.dir/gears.c.o


# Object files for target Gears
Gears_OBJECTS = \
"CMakeFiles/Gears.dir/gears.c.o"

# External object files for target Gears
Gears_EXTERNAL_OBJECTS =

examples/Gears.app/Contents/MacOS/Gears: examples/CMakeFiles/Gears.dir/gears.c.o
examples/Gears.app/Contents/MacOS/Gears: examples/CMakeFiles/Gears.dir/build.make
examples/Gears.app/Contents/MacOS/Gears: src/libglfw3.a
examples/Gears.app/Contents/MacOS/Gears: examples/CMakeFiles/Gears.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable Gears.app/Contents/MacOS/Gears"
	cd /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Gears.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/Gears.dir/build: examples/Gears.app/Contents/MacOS/Gears

.PHONY : examples/CMakeFiles/Gears.dir/build

examples/CMakeFiles/Gears.dir/requires: examples/CMakeFiles/Gears.dir/gears.c.o.requires

.PHONY : examples/CMakeFiles/Gears.dir/requires

examples/CMakeFiles/Gears.dir/clean:
	cd /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples && $(CMAKE_COMMAND) -P CMakeFiles/Gears.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/Gears.dir/clean

examples/CMakeFiles/Gears.dir/depend:
	cd /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples /nfs/zfs-student-3/users/2014/achazal/bombermanRedo/external_librairies/glfw/examples/CMakeFiles/Gears.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/Gears.dir/depend


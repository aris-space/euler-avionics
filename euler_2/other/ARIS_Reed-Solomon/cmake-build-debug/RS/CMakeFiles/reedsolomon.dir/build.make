# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.17

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

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.2.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.2.1\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug

# Include any dependencies generated for this target.
include RS/CMakeFiles/reedsolomon.dir/depend.make

# Include the progress variables for this target.
include RS/CMakeFiles/reedsolomon.dir/progress.make

# Include the compile flags for this target's objects.
include RS/CMakeFiles/reedsolomon.dir/flags.make

RS/CMakeFiles/reedsolomon.dir/RS.c.obj: RS/CMakeFiles/reedsolomon.dir/flags.make
RS/CMakeFiles/reedsolomon.dir/RS.c.obj: ../RS/RS.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object RS/CMakeFiles/reedsolomon.dir/RS.c.obj"
	cd /d C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\RS && C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\reedsolomon.dir\RS.c.obj   -c C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\RS\RS.c

RS/CMakeFiles/reedsolomon.dir/RS.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/reedsolomon.dir/RS.c.i"
	cd /d C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\RS && C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\RS\RS.c > CMakeFiles\reedsolomon.dir\RS.c.i

RS/CMakeFiles/reedsolomon.dir/RS.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/reedsolomon.dir/RS.c.s"
	cd /d C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\RS && C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\RS\RS.c -o CMakeFiles\reedsolomon.dir\RS.c.s

# Object files for target reedsolomon
reedsolomon_OBJECTS = \
"CMakeFiles/reedsolomon.dir/RS.c.obj"

# External object files for target reedsolomon
reedsolomon_EXTERNAL_OBJECTS =

RS/libreedsolomon.a: RS/CMakeFiles/reedsolomon.dir/RS.c.obj
RS/libreedsolomon.a: RS/CMakeFiles/reedsolomon.dir/build.make
RS/libreedsolomon.a: RS/CMakeFiles/reedsolomon.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libreedsolomon.a"
	cd /d C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\RS && $(CMAKE_COMMAND) -P CMakeFiles\reedsolomon.dir\cmake_clean_target.cmake
	cd /d C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\RS && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\reedsolomon.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
RS/CMakeFiles/reedsolomon.dir/build: RS/libreedsolomon.a

.PHONY : RS/CMakeFiles/reedsolomon.dir/build

RS/CMakeFiles/reedsolomon.dir/clean:
	cd /d C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\RS && $(CMAKE_COMMAND) -P CMakeFiles\reedsolomon.dir\cmake_clean.cmake
.PHONY : RS/CMakeFiles/reedsolomon.dir/clean

RS/CMakeFiles/reedsolomon.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\RS C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\RS C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\RS\CMakeFiles\reedsolomon.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : RS/CMakeFiles/reedsolomon.dir/depend


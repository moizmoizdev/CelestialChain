# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.29

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = D:\Strawberry\c\bin\cmake.exe

# The command to remove a file.
RM = D:\Strawberry\c\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "D:\Distributed BlockChain\api"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "D:\Distributed BlockChain\api\build"

# Include any dependencies generated for this target.
include CMakeFiles/CelestialChainAPI.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/CelestialChainAPI.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/CelestialChainAPI.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CelestialChainAPI.dir/flags.make

CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.obj: CMakeFiles/CelestialChainAPI.dir/flags.make
CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.obj: CMakeFiles/CelestialChainAPI.dir/includes_CXX.rsp
CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.obj: D:/Distributed\ BlockChain/api/CelestialChainAPI.cpp
CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.obj: CMakeFiles/CelestialChainAPI.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="D:\Distributed BlockChain\api\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.obj"
	D:\Strawberry\c\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.obj -MF CMakeFiles\CelestialChainAPI.dir\CelestialChainAPI.cpp.obj.d -o CMakeFiles\CelestialChainAPI.dir\CelestialChainAPI.cpp.obj -c "D:\Distributed BlockChain\api\CelestialChainAPI.cpp"

CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.i"
	D:\Strawberry\c\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "D:\Distributed BlockChain\api\CelestialChainAPI.cpp" > CMakeFiles\CelestialChainAPI.dir\CelestialChainAPI.cpp.i

CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.s"
	D:\Strawberry\c\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "D:\Distributed BlockChain\api\CelestialChainAPI.cpp" -o CMakeFiles\CelestialChainAPI.dir\CelestialChainAPI.cpp.s

# Object files for target CelestialChainAPI
CelestialChainAPI_OBJECTS = \
"CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.obj"

# External object files for target CelestialChainAPI
CelestialChainAPI_EXTERNAL_OBJECTS =

libCelestialChainAPI.a: CMakeFiles/CelestialChainAPI.dir/CelestialChainAPI.cpp.obj
libCelestialChainAPI.a: CMakeFiles/CelestialChainAPI.dir/build.make
libCelestialChainAPI.a: CMakeFiles/CelestialChainAPI.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="D:\Distributed BlockChain\api\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libCelestialChainAPI.a"
	$(CMAKE_COMMAND) -P CMakeFiles\CelestialChainAPI.dir\cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\CelestialChainAPI.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CelestialChainAPI.dir/build: libCelestialChainAPI.a
.PHONY : CMakeFiles/CelestialChainAPI.dir/build

CMakeFiles/CelestialChainAPI.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\CelestialChainAPI.dir\cmake_clean.cmake
.PHONY : CMakeFiles/CelestialChainAPI.dir/clean

CMakeFiles/CelestialChainAPI.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "D:\Distributed BlockChain\api" "D:\Distributed BlockChain\api" "D:\Distributed BlockChain\api\build" "D:\Distributed BlockChain\api\build" "D:\Distributed BlockChain\api\build\CMakeFiles\CelestialChainAPI.dir\DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/CelestialChainAPI.dir/depend


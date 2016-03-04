# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:
.PHONY : .NOTPARALLEL

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
CMAKE_SOURCE_DIR = /home/snn/BSim

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/snn/BSim

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/snn/BSim/CMakeFiles /home/snn/BSim/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/snn/BSim/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named bsim

# Build rule for target.
bsim: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 bsim
.PHONY : bsim

# fast build rule for target.
bsim/fast:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/build
.PHONY : bsim/fast

#=============================================================================
# Target rules for targets named bsim_gpu

# Build rule for target.
bsim_gpu: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 bsim_gpu
.PHONY : bsim_gpu

# fast build rule for target.
bsim_gpu/fast:
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/build
.PHONY : bsim_gpu/fast

#=============================================================================
# Target rules for targets named generater_test

# Build rule for target.
generater_test: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 generater_test
.PHONY : generater_test

# fast build rule for target.
generater_test/fast:
	$(MAKE) -f CMakeFiles/generater_test.dir/build.make CMakeFiles/generater_test.dir/build
.PHONY : generater_test/fast

#=============================================================================
# Target rules for targets named gpusim_test

# Build rule for target.
gpusim_test: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 gpusim_test
.PHONY : gpusim_test

# fast build rule for target.
gpusim_test/fast:
	$(MAKE) -f CMakeFiles/gpusim_test.dir/build.make CMakeFiles/gpusim_test.dir/build
.PHONY : gpusim_test/fast

#=============================================================================
# Target rules for targets named neural_test

# Build rule for target.
neural_test: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 neural_test
.PHONY : neural_test

# fast build rule for target.
neural_test/fast:
	$(MAKE) -f CMakeFiles/neural_test.dir/build.make CMakeFiles/neural_test.dir/build
.PHONY : neural_test/fast

#=============================================================================
# Target rules for targets named read_write_test

# Build rule for target.
read_write_test: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 read_write_test
.PHONY : read_write_test

# fast build rule for target.
read_write_test/fast:
	$(MAKE) -f CMakeFiles/read_write_test.dir/build.make CMakeFiles/read_write_test.dir/build
.PHONY : read_write_test/fast

#=============================================================================
# Target rules for targets named snn_from_file

# Build rule for target.
snn_from_file: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 snn_from_file
.PHONY : snn_from_file

# fast build rule for target.
snn_from_file/fast:
	$(MAKE) -f CMakeFiles/snn_from_file.dir/build.make CMakeFiles/snn_from_file.dir/build
.PHONY : snn_from_file/fast

#=============================================================================
# Target rules for targets named synapse_test

# Build rule for target.
synapse_test: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 synapse_test
.PHONY : synapse_test

# fast build rule for target.
synapse_test/fast:
	$(MAKE) -f CMakeFiles/synapse_test.dir/build.make CMakeFiles/synapse_test.dir/build
.PHONY : synapse_test/fast

src/AlphaSynapse.o: src/AlphaSynapse.cpp.o
.PHONY : src/AlphaSynapse.o

# target to build an object file
src/AlphaSynapse.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/AlphaSynapse.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/AlphaSynapse.cpp.o
.PHONY : src/AlphaSynapse.cpp.o

src/AlphaSynapse.i: src/AlphaSynapse.cpp.i
.PHONY : src/AlphaSynapse.i

# target to preprocess a source file
src/AlphaSynapse.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/AlphaSynapse.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/AlphaSynapse.cpp.i
.PHONY : src/AlphaSynapse.cpp.i

src/AlphaSynapse.s: src/AlphaSynapse.cpp.s
.PHONY : src/AlphaSynapse.s

# target to generate assembly for a file
src/AlphaSynapse.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/AlphaSynapse.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/AlphaSynapse.cpp.s
.PHONY : src/AlphaSynapse.cpp.s

src/ExpSynapse.o: src/ExpSynapse.cpp.o
.PHONY : src/ExpSynapse.o

# target to build an object file
src/ExpSynapse.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/ExpSynapse.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/ExpSynapse.cpp.o
.PHONY : src/ExpSynapse.cpp.o

src/ExpSynapse.i: src/ExpSynapse.cpp.i
.PHONY : src/ExpSynapse.i

# target to preprocess a source file
src/ExpSynapse.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/ExpSynapse.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/ExpSynapse.cpp.i
.PHONY : src/ExpSynapse.cpp.i

src/ExpSynapse.s: src/ExpSynapse.cpp.s
.PHONY : src/ExpSynapse.s

# target to generate assembly for a file
src/ExpSynapse.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/ExpSynapse.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/ExpSynapse.cpp.s
.PHONY : src/ExpSynapse.cpp.s

src/GAlphaSynapses.o: src/GAlphaSynapses.cpp.o
.PHONY : src/GAlphaSynapses.o

# target to build an object file
src/GAlphaSynapses.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/GAlphaSynapses.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/GAlphaSynapses.cpp.o
.PHONY : src/GAlphaSynapses.cpp.o

src/GAlphaSynapses.i: src/GAlphaSynapses.cpp.i
.PHONY : src/GAlphaSynapses.i

# target to preprocess a source file
src/GAlphaSynapses.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/GAlphaSynapses.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/GAlphaSynapses.cpp.i
.PHONY : src/GAlphaSynapses.cpp.i

src/GAlphaSynapses.s: src/GAlphaSynapses.cpp.s
.PHONY : src/GAlphaSynapses.s

# target to generate assembly for a file
src/GAlphaSynapses.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/GAlphaSynapses.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/GAlphaSynapses.cpp.s
.PHONY : src/GAlphaSynapses.cpp.s

src/GExpSynapses.o: src/GExpSynapses.cpp.o
.PHONY : src/GExpSynapses.o

# target to build an object file
src/GExpSynapses.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/GExpSynapses.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/GExpSynapses.cpp.o
.PHONY : src/GExpSynapses.cpp.o

src/GExpSynapses.i: src/GExpSynapses.cpp.i
.PHONY : src/GExpSynapses.i

# target to preprocess a source file
src/GExpSynapses.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/GExpSynapses.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/GExpSynapses.cpp.i
.PHONY : src/GExpSynapses.cpp.i

src/GExpSynapses.s: src/GExpSynapses.cpp.s
.PHONY : src/GExpSynapses.s

# target to generate assembly for a file
src/GExpSynapses.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/GExpSynapses.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/GExpSynapses.cpp.s
.PHONY : src/GExpSynapses.cpp.s

src/GLIFNeurons.o: src/GLIFNeurons.cpp.o
.PHONY : src/GLIFNeurons.o

# target to build an object file
src/GLIFNeurons.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/GLIFNeurons.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/GLIFNeurons.cpp.o
.PHONY : src/GLIFNeurons.cpp.o

src/GLIFNeurons.i: src/GLIFNeurons.cpp.i
.PHONY : src/GLIFNeurons.i

# target to preprocess a source file
src/GLIFNeurons.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/GLIFNeurons.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/GLIFNeurons.cpp.i
.PHONY : src/GLIFNeurons.cpp.i

src/GLIFNeurons.s: src/GLIFNeurons.cpp.s
.PHONY : src/GLIFNeurons.s

# target to generate assembly for a file
src/GLIFNeurons.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/GLIFNeurons.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/GLIFNeurons.cpp.s
.PHONY : src/GLIFNeurons.cpp.s

src/Generator.o: src/Generator.cpp.o
.PHONY : src/Generator.o

# target to build an object file
src/Generator.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/Generator.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/Generator.cpp.o
.PHONY : src/Generator.cpp.o

src/Generator.i: src/Generator.cpp.i
.PHONY : src/Generator.i

# target to preprocess a source file
src/Generator.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/Generator.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/Generator.cpp.i
.PHONY : src/Generator.cpp.i

src/Generator.s: src/Generator.cpp.s
.PHONY : src/Generator.s

# target to generate assembly for a file
src/Generator.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/Generator.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/Generator.cpp.s
.PHONY : src/Generator.cpp.s

src/ID.o: src/ID.cpp.o
.PHONY : src/ID.o

# target to build an object file
src/ID.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/ID.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/ID.cpp.o
.PHONY : src/ID.cpp.o

src/ID.i: src/ID.cpp.i
.PHONY : src/ID.i

# target to preprocess a source file
src/ID.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/ID.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/ID.cpp.i
.PHONY : src/ID.cpp.i

src/ID.s: src/ID.cpp.s
.PHONY : src/ID.s

# target to generate assembly for a file
src/ID.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/ID.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/ID.cpp.s
.PHONY : src/ID.cpp.s

src/IDPool.o: src/IDPool.cpp.o
.PHONY : src/IDPool.o

# target to build an object file
src/IDPool.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/IDPool.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/IDPool.cpp.o
.PHONY : src/IDPool.cpp.o

src/IDPool.i: src/IDPool.cpp.i
.PHONY : src/IDPool.i

# target to preprocess a source file
src/IDPool.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/IDPool.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/IDPool.cpp.i
.PHONY : src/IDPool.cpp.i

src/IDPool.s: src/IDPool.cpp.s
.PHONY : src/IDPool.s

# target to generate assembly for a file
src/IDPool.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/IDPool.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/IDPool.cpp.s
.PHONY : src/IDPool.cpp.s

src/IF_curr_exp.o: src/IF_curr_exp.cpp.o
.PHONY : src/IF_curr_exp.o

# target to build an object file
src/IF_curr_exp.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/IF_curr_exp.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/IF_curr_exp.cpp.o
.PHONY : src/IF_curr_exp.cpp.o

src/IF_curr_exp.i: src/IF_curr_exp.cpp.i
.PHONY : src/IF_curr_exp.i

# target to preprocess a source file
src/IF_curr_exp.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/IF_curr_exp.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/IF_curr_exp.cpp.i
.PHONY : src/IF_curr_exp.cpp.i

src/IF_curr_exp.s: src/IF_curr_exp.cpp.s
.PHONY : src/IF_curr_exp.s

# target to generate assembly for a file
src/IF_curr_exp.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/IF_curr_exp.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/IF_curr_exp.cpp.s
.PHONY : src/IF_curr_exp.cpp.s

src/InputNeuron.o: src/InputNeuron.cpp.o
.PHONY : src/InputNeuron.o

# target to build an object file
src/InputNeuron.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/InputNeuron.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/InputNeuron.cpp.o
.PHONY : src/InputNeuron.cpp.o

src/InputNeuron.i: src/InputNeuron.cpp.i
.PHONY : src/InputNeuron.i

# target to preprocess a source file
src/InputNeuron.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/InputNeuron.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/InputNeuron.cpp.i
.PHONY : src/InputNeuron.cpp.i

src/InputNeuron.s: src/InputNeuron.cpp.s
.PHONY : src/InputNeuron.s

# target to generate assembly for a file
src/InputNeuron.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/InputNeuron.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/InputNeuron.cpp.s
.PHONY : src/InputNeuron.cpp.s

src/LIFNeuron.o: src/LIFNeuron.cpp.o
.PHONY : src/LIFNeuron.o

# target to build an object file
src/LIFNeuron.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/LIFNeuron.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/LIFNeuron.cpp.o
.PHONY : src/LIFNeuron.cpp.o

src/LIFNeuron.i: src/LIFNeuron.cpp.i
.PHONY : src/LIFNeuron.i

# target to preprocess a source file
src/LIFNeuron.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/LIFNeuron.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/LIFNeuron.cpp.i
.PHONY : src/LIFNeuron.cpp.i

src/LIFNeuron.s: src/LIFNeuron.cpp.s
.PHONY : src/LIFNeuron.s

# target to generate assembly for a file
src/LIFNeuron.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/LIFNeuron.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/LIFNeuron.cpp.s
.PHONY : src/LIFNeuron.cpp.s

src/LowpassSynapse.o: src/LowpassSynapse.cpp.o
.PHONY : src/LowpassSynapse.o

# target to build an object file
src/LowpassSynapse.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/LowpassSynapse.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/LowpassSynapse.cpp.o
.PHONY : src/LowpassSynapse.cpp.o

src/LowpassSynapse.i: src/LowpassSynapse.cpp.i
.PHONY : src/LowpassSynapse.i

# target to preprocess a source file
src/LowpassSynapse.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/LowpassSynapse.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/LowpassSynapse.cpp.i
.PHONY : src/LowpassSynapse.cpp.i

src/LowpassSynapse.s: src/LowpassSynapse.cpp.s
.PHONY : src/LowpassSynapse.s

# target to generate assembly for a file
src/LowpassSynapse.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/LowpassSynapse.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/LowpassSynapse.cpp.s
.PHONY : src/LowpassSynapse.cpp.s

src/NengoNeuron.o: src/NengoNeuron.cpp.o
.PHONY : src/NengoNeuron.o

# target to build an object file
src/NengoNeuron.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/NengoNeuron.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/NengoNeuron.cpp.o
.PHONY : src/NengoNeuron.cpp.o

src/NengoNeuron.i: src/NengoNeuron.cpp.i
.PHONY : src/NengoNeuron.i

# target to preprocess a source file
src/NengoNeuron.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/NengoNeuron.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/NengoNeuron.cpp.i
.PHONY : src/NengoNeuron.cpp.i

src/NengoNeuron.s: src/NengoNeuron.cpp.s
.PHONY : src/NengoNeuron.s

# target to generate assembly for a file
src/NengoNeuron.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/NengoNeuron.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/NengoNeuron.cpp.s
.PHONY : src/NengoNeuron.cpp.s

src/Network.o: src/Network.cpp.o
.PHONY : src/Network.o

# target to build an object file
src/Network.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/Network.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/Network.cpp.o
.PHONY : src/Network.cpp.o

src/Network.i: src/Network.cpp.i
.PHONY : src/Network.i

# target to preprocess a source file
src/Network.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/Network.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/Network.cpp.i
.PHONY : src/Network.cpp.i

src/Network.s: src/Network.cpp.s
.PHONY : src/Network.s

# target to generate assembly for a file
src/Network.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/Network.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/Network.cpp.s
.PHONY : src/Network.cpp.s

src/NeuronBase.o: src/NeuronBase.cpp.o
.PHONY : src/NeuronBase.o

# target to build an object file
src/NeuronBase.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/NeuronBase.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/NeuronBase.cpp.o
.PHONY : src/NeuronBase.cpp.o

src/NeuronBase.i: src/NeuronBase.cpp.i
.PHONY : src/NeuronBase.i

# target to preprocess a source file
src/NeuronBase.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/NeuronBase.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/NeuronBase.cpp.i
.PHONY : src/NeuronBase.cpp.i

src/NeuronBase.s: src/NeuronBase.cpp.s
.PHONY : src/NeuronBase.s

# target to generate assembly for a file
src/NeuronBase.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/NeuronBase.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/NeuronBase.cpp.s
.PHONY : src/NeuronBase.cpp.s

src/PopulationBase.o: src/PopulationBase.cpp.o
.PHONY : src/PopulationBase.o

# target to build an object file
src/PopulationBase.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/PopulationBase.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/PopulationBase.cpp.o
.PHONY : src/PopulationBase.cpp.o

src/PopulationBase.i: src/PopulationBase.cpp.i
.PHONY : src/PopulationBase.i

# target to preprocess a source file
src/PopulationBase.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/PopulationBase.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/PopulationBase.cpp.i
.PHONY : src/PopulationBase.cpp.i

src/PopulationBase.s: src/PopulationBase.cpp.s
.PHONY : src/PopulationBase.s

# target to generate assembly for a file
src/PopulationBase.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/PopulationBase.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/PopulationBase.cpp.s
.PHONY : src/PopulationBase.cpp.s

src/PossionNeuron.o: src/PossionNeuron.cpp.o
.PHONY : src/PossionNeuron.o

# target to build an object file
src/PossionNeuron.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/PossionNeuron.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/PossionNeuron.cpp.o
.PHONY : src/PossionNeuron.cpp.o

src/PossionNeuron.i: src/PossionNeuron.cpp.i
.PHONY : src/PossionNeuron.i

# target to preprocess a source file
src/PossionNeuron.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/PossionNeuron.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/PossionNeuron.cpp.i
.PHONY : src/PossionNeuron.cpp.i

src/PossionNeuron.s: src/PossionNeuron.cpp.s
.PHONY : src/PossionNeuron.s

# target to generate assembly for a file
src/PossionNeuron.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/PossionNeuron.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/PossionNeuron.cpp.s
.PHONY : src/PossionNeuron.cpp.s

src/ProbeNeuron.o: src/ProbeNeuron.cpp.o
.PHONY : src/ProbeNeuron.o

# target to build an object file
src/ProbeNeuron.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/ProbeNeuron.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/ProbeNeuron.cpp.o
.PHONY : src/ProbeNeuron.cpp.o

src/ProbeNeuron.i: src/ProbeNeuron.cpp.i
.PHONY : src/ProbeNeuron.i

# target to preprocess a source file
src/ProbeNeuron.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/ProbeNeuron.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/ProbeNeuron.cpp.i
.PHONY : src/ProbeNeuron.cpp.i

src/ProbeNeuron.s: src/ProbeNeuron.cpp.s
.PHONY : src/ProbeNeuron.s

# target to generate assembly for a file
src/ProbeNeuron.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/ProbeNeuron.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/ProbeNeuron.cpp.s
.PHONY : src/ProbeNeuron.cpp.s

src/SNNReader.o: src/SNNReader.cpp.o
.PHONY : src/SNNReader.o

# target to build an object file
src/SNNReader.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SNNReader.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SNNReader.cpp.o
.PHONY : src/SNNReader.cpp.o

src/SNNReader.i: src/SNNReader.cpp.i
.PHONY : src/SNNReader.i

# target to preprocess a source file
src/SNNReader.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SNNReader.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SNNReader.cpp.i
.PHONY : src/SNNReader.cpp.i

src/SNNReader.s: src/SNNReader.cpp.s
.PHONY : src/SNNReader.s

# target to generate assembly for a file
src/SNNReader.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SNNReader.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SNNReader.cpp.s
.PHONY : src/SNNReader.cpp.s

src/SNNWriter.o: src/SNNWriter.cpp.o
.PHONY : src/SNNWriter.o

# target to build an object file
src/SNNWriter.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SNNWriter.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SNNWriter.cpp.o
.PHONY : src/SNNWriter.cpp.o

src/SNNWriter.i: src/SNNWriter.cpp.i
.PHONY : src/SNNWriter.i

# target to preprocess a source file
src/SNNWriter.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SNNWriter.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SNNWriter.cpp.i
.PHONY : src/SNNWriter.cpp.i

src/SNNWriter.s: src/SNNWriter.cpp.s
.PHONY : src/SNNWriter.s

# target to generate assembly for a file
src/SNNWriter.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SNNWriter.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SNNWriter.cpp.s
.PHONY : src/SNNWriter.cpp.s

src/SimulatorBase.o: src/SimulatorBase.cpp.o
.PHONY : src/SimulatorBase.o

# target to build an object file
src/SimulatorBase.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SimulatorBase.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SimulatorBase.cpp.o
.PHONY : src/SimulatorBase.cpp.o

src/SimulatorBase.i: src/SimulatorBase.cpp.i
.PHONY : src/SimulatorBase.i

# target to preprocess a source file
src/SimulatorBase.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SimulatorBase.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SimulatorBase.cpp.i
.PHONY : src/SimulatorBase.cpp.i

src/SimulatorBase.s: src/SimulatorBase.cpp.s
.PHONY : src/SimulatorBase.s

# target to generate assembly for a file
src/SimulatorBase.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SimulatorBase.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SimulatorBase.cpp.s
.PHONY : src/SimulatorBase.cpp.s

src/SingleThreadSimulator.o: src/SingleThreadSimulator.cpp.o
.PHONY : src/SingleThreadSimulator.o

# target to build an object file
src/SingleThreadSimulator.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SingleThreadSimulator.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SingleThreadSimulator.cpp.o
.PHONY : src/SingleThreadSimulator.cpp.o

src/SingleThreadSimulator.i: src/SingleThreadSimulator.cpp.i
.PHONY : src/SingleThreadSimulator.i

# target to preprocess a source file
src/SingleThreadSimulator.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SingleThreadSimulator.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SingleThreadSimulator.cpp.i
.PHONY : src/SingleThreadSimulator.cpp.i

src/SingleThreadSimulator.s: src/SingleThreadSimulator.cpp.s
.PHONY : src/SingleThreadSimulator.s

# target to generate assembly for a file
src/SingleThreadSimulator.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SingleThreadSimulator.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SingleThreadSimulator.cpp.s
.PHONY : src/SingleThreadSimulator.cpp.s

src/SynapseBase.o: src/SynapseBase.cpp.o
.PHONY : src/SynapseBase.o

# target to build an object file
src/SynapseBase.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SynapseBase.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SynapseBase.cpp.o
.PHONY : src/SynapseBase.cpp.o

src/SynapseBase.i: src/SynapseBase.cpp.i
.PHONY : src/SynapseBase.i

# target to preprocess a source file
src/SynapseBase.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SynapseBase.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SynapseBase.cpp.i
.PHONY : src/SynapseBase.cpp.i

src/SynapseBase.s: src/SynapseBase.cpp.s
.PHONY : src/SynapseBase.s

# target to generate assembly for a file
src/SynapseBase.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/SynapseBase.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/SynapseBase.cpp.s
.PHONY : src/SynapseBase.cpp.s

src/utils/json/json.o: src/utils/json/json.cpp.o
.PHONY : src/utils/json/json.o

# target to build an object file
src/utils/json/json.cpp.o:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/utils/json/json.cpp.o
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/utils/json/json.cpp.o
.PHONY : src/utils/json/json.cpp.o

src/utils/json/json.i: src/utils/json/json.cpp.i
.PHONY : src/utils/json/json.i

# target to preprocess a source file
src/utils/json/json.cpp.i:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/utils/json/json.cpp.i
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/utils/json/json.cpp.i
.PHONY : src/utils/json/json.cpp.i

src/utils/json/json.s: src/utils/json/json.cpp.s
.PHONY : src/utils/json/json.s

# target to generate assembly for a file
src/utils/json/json.cpp.s:
	$(MAKE) -f CMakeFiles/bsim.dir/build.make CMakeFiles/bsim.dir/src/utils/json/json.cpp.s
	$(MAKE) -f CMakeFiles/bsim_gpu.dir/build.make CMakeFiles/bsim_gpu.dir/src/utils/json/json.cpp.s
.PHONY : src/utils/json/json.cpp.s

test/cpu/generater_test.o: test/cpu/generater_test.cpp.o
.PHONY : test/cpu/generater_test.o

# target to build an object file
test/cpu/generater_test.cpp.o:
	$(MAKE) -f CMakeFiles/generater_test.dir/build.make CMakeFiles/generater_test.dir/test/cpu/generater_test.cpp.o
.PHONY : test/cpu/generater_test.cpp.o

test/cpu/generater_test.i: test/cpu/generater_test.cpp.i
.PHONY : test/cpu/generater_test.i

# target to preprocess a source file
test/cpu/generater_test.cpp.i:
	$(MAKE) -f CMakeFiles/generater_test.dir/build.make CMakeFiles/generater_test.dir/test/cpu/generater_test.cpp.i
.PHONY : test/cpu/generater_test.cpp.i

test/cpu/generater_test.s: test/cpu/generater_test.cpp.s
.PHONY : test/cpu/generater_test.s

# target to generate assembly for a file
test/cpu/generater_test.cpp.s:
	$(MAKE) -f CMakeFiles/generater_test.dir/build.make CMakeFiles/generater_test.dir/test/cpu/generater_test.cpp.s
.PHONY : test/cpu/generater_test.cpp.s

test/cpu/neural_test.o: test/cpu/neural_test.cpp.o
.PHONY : test/cpu/neural_test.o

# target to build an object file
test/cpu/neural_test.cpp.o:
	$(MAKE) -f CMakeFiles/neural_test.dir/build.make CMakeFiles/neural_test.dir/test/cpu/neural_test.cpp.o
.PHONY : test/cpu/neural_test.cpp.o

test/cpu/neural_test.i: test/cpu/neural_test.cpp.i
.PHONY : test/cpu/neural_test.i

# target to preprocess a source file
test/cpu/neural_test.cpp.i:
	$(MAKE) -f CMakeFiles/neural_test.dir/build.make CMakeFiles/neural_test.dir/test/cpu/neural_test.cpp.i
.PHONY : test/cpu/neural_test.cpp.i

test/cpu/neural_test.s: test/cpu/neural_test.cpp.s
.PHONY : test/cpu/neural_test.s

# target to generate assembly for a file
test/cpu/neural_test.cpp.s:
	$(MAKE) -f CMakeFiles/neural_test.dir/build.make CMakeFiles/neural_test.dir/test/cpu/neural_test.cpp.s
.PHONY : test/cpu/neural_test.cpp.s

test/cpu/read_write_test.o: test/cpu/read_write_test.cpp.o
.PHONY : test/cpu/read_write_test.o

# target to build an object file
test/cpu/read_write_test.cpp.o:
	$(MAKE) -f CMakeFiles/read_write_test.dir/build.make CMakeFiles/read_write_test.dir/test/cpu/read_write_test.cpp.o
.PHONY : test/cpu/read_write_test.cpp.o

test/cpu/read_write_test.i: test/cpu/read_write_test.cpp.i
.PHONY : test/cpu/read_write_test.i

# target to preprocess a source file
test/cpu/read_write_test.cpp.i:
	$(MAKE) -f CMakeFiles/read_write_test.dir/build.make CMakeFiles/read_write_test.dir/test/cpu/read_write_test.cpp.i
.PHONY : test/cpu/read_write_test.cpp.i

test/cpu/read_write_test.s: test/cpu/read_write_test.cpp.s
.PHONY : test/cpu/read_write_test.s

# target to generate assembly for a file
test/cpu/read_write_test.cpp.s:
	$(MAKE) -f CMakeFiles/read_write_test.dir/build.make CMakeFiles/read_write_test.dir/test/cpu/read_write_test.cpp.s
.PHONY : test/cpu/read_write_test.cpp.s

test/cpu/snn_from_file.o: test/cpu/snn_from_file.cpp.o
.PHONY : test/cpu/snn_from_file.o

# target to build an object file
test/cpu/snn_from_file.cpp.o:
	$(MAKE) -f CMakeFiles/snn_from_file.dir/build.make CMakeFiles/snn_from_file.dir/test/cpu/snn_from_file.cpp.o
.PHONY : test/cpu/snn_from_file.cpp.o

test/cpu/snn_from_file.i: test/cpu/snn_from_file.cpp.i
.PHONY : test/cpu/snn_from_file.i

# target to preprocess a source file
test/cpu/snn_from_file.cpp.i:
	$(MAKE) -f CMakeFiles/snn_from_file.dir/build.make CMakeFiles/snn_from_file.dir/test/cpu/snn_from_file.cpp.i
.PHONY : test/cpu/snn_from_file.cpp.i

test/cpu/snn_from_file.s: test/cpu/snn_from_file.cpp.s
.PHONY : test/cpu/snn_from_file.s

# target to generate assembly for a file
test/cpu/snn_from_file.cpp.s:
	$(MAKE) -f CMakeFiles/snn_from_file.dir/build.make CMakeFiles/snn_from_file.dir/test/cpu/snn_from_file.cpp.s
.PHONY : test/cpu/snn_from_file.cpp.s

test/cpu/synapse_test.o: test/cpu/synapse_test.cpp.o
.PHONY : test/cpu/synapse_test.o

# target to build an object file
test/cpu/synapse_test.cpp.o:
	$(MAKE) -f CMakeFiles/synapse_test.dir/build.make CMakeFiles/synapse_test.dir/test/cpu/synapse_test.cpp.o
.PHONY : test/cpu/synapse_test.cpp.o

test/cpu/synapse_test.i: test/cpu/synapse_test.cpp.i
.PHONY : test/cpu/synapse_test.i

# target to preprocess a source file
test/cpu/synapse_test.cpp.i:
	$(MAKE) -f CMakeFiles/synapse_test.dir/build.make CMakeFiles/synapse_test.dir/test/cpu/synapse_test.cpp.i
.PHONY : test/cpu/synapse_test.cpp.i

test/cpu/synapse_test.s: test/cpu/synapse_test.cpp.s
.PHONY : test/cpu/synapse_test.s

# target to generate assembly for a file
test/cpu/synapse_test.cpp.s:
	$(MAKE) -f CMakeFiles/synapse_test.dir/build.make CMakeFiles/synapse_test.dir/test/cpu/synapse_test.cpp.s
.PHONY : test/cpu/synapse_test.cpp.s

test/gpu/gpusim_test.o: test/gpu/gpusim_test.cpp.o
.PHONY : test/gpu/gpusim_test.o

# target to build an object file
test/gpu/gpusim_test.cpp.o:
	$(MAKE) -f CMakeFiles/gpusim_test.dir/build.make CMakeFiles/gpusim_test.dir/test/gpu/gpusim_test.cpp.o
.PHONY : test/gpu/gpusim_test.cpp.o

test/gpu/gpusim_test.i: test/gpu/gpusim_test.cpp.i
.PHONY : test/gpu/gpusim_test.i

# target to preprocess a source file
test/gpu/gpusim_test.cpp.i:
	$(MAKE) -f CMakeFiles/gpusim_test.dir/build.make CMakeFiles/gpusim_test.dir/test/gpu/gpusim_test.cpp.i
.PHONY : test/gpu/gpusim_test.cpp.i

test/gpu/gpusim_test.s: test/gpu/gpusim_test.cpp.s
.PHONY : test/gpu/gpusim_test.s

# target to generate assembly for a file
test/gpu/gpusim_test.cpp.s:
	$(MAKE) -f CMakeFiles/gpusim_test.dir/build.make CMakeFiles/gpusim_test.dir/test/gpu/gpusim_test.cpp.s
.PHONY : test/gpu/gpusim_test.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... bsim"
	@echo "... bsim_gpu"
	@echo "... edit_cache"
	@echo "... generater_test"
	@echo "... gpusim_test"
	@echo "... neural_test"
	@echo "... read_write_test"
	@echo "... rebuild_cache"
	@echo "... snn_from_file"
	@echo "... synapse_test"
	@echo "... src/AlphaSynapse.o"
	@echo "... src/AlphaSynapse.i"
	@echo "... src/AlphaSynapse.s"
	@echo "... src/ExpSynapse.o"
	@echo "... src/ExpSynapse.i"
	@echo "... src/ExpSynapse.s"
	@echo "... src/GAlphaSynapses.o"
	@echo "... src/GAlphaSynapses.i"
	@echo "... src/GAlphaSynapses.s"
	@echo "... src/GExpSynapses.o"
	@echo "... src/GExpSynapses.i"
	@echo "... src/GExpSynapses.s"
	@echo "... src/GLIFNeurons.o"
	@echo "... src/GLIFNeurons.i"
	@echo "... src/GLIFNeurons.s"
	@echo "... src/Generator.o"
	@echo "... src/Generator.i"
	@echo "... src/Generator.s"
	@echo "... src/ID.o"
	@echo "... src/ID.i"
	@echo "... src/ID.s"
	@echo "... src/IDPool.o"
	@echo "... src/IDPool.i"
	@echo "... src/IDPool.s"
	@echo "... src/IF_curr_exp.o"
	@echo "... src/IF_curr_exp.i"
	@echo "... src/IF_curr_exp.s"
	@echo "... src/InputNeuron.o"
	@echo "... src/InputNeuron.i"
	@echo "... src/InputNeuron.s"
	@echo "... src/LIFNeuron.o"
	@echo "... src/LIFNeuron.i"
	@echo "... src/LIFNeuron.s"
	@echo "... src/LowpassSynapse.o"
	@echo "... src/LowpassSynapse.i"
	@echo "... src/LowpassSynapse.s"
	@echo "... src/NengoNeuron.o"
	@echo "... src/NengoNeuron.i"
	@echo "... src/NengoNeuron.s"
	@echo "... src/Network.o"
	@echo "... src/Network.i"
	@echo "... src/Network.s"
	@echo "... src/NeuronBase.o"
	@echo "... src/NeuronBase.i"
	@echo "... src/NeuronBase.s"
	@echo "... src/PopulationBase.o"
	@echo "... src/PopulationBase.i"
	@echo "... src/PopulationBase.s"
	@echo "... src/PossionNeuron.o"
	@echo "... src/PossionNeuron.i"
	@echo "... src/PossionNeuron.s"
	@echo "... src/ProbeNeuron.o"
	@echo "... src/ProbeNeuron.i"
	@echo "... src/ProbeNeuron.s"
	@echo "... src/SNNReader.o"
	@echo "... src/SNNReader.i"
	@echo "... src/SNNReader.s"
	@echo "... src/SNNWriter.o"
	@echo "... src/SNNWriter.i"
	@echo "... src/SNNWriter.s"
	@echo "... src/SimulatorBase.o"
	@echo "... src/SimulatorBase.i"
	@echo "... src/SimulatorBase.s"
	@echo "... src/SingleThreadSimulator.o"
	@echo "... src/SingleThreadSimulator.i"
	@echo "... src/SingleThreadSimulator.s"
	@echo "... src/SynapseBase.o"
	@echo "... src/SynapseBase.i"
	@echo "... src/SynapseBase.s"
	@echo "... src/utils/json/json.o"
	@echo "... src/utils/json/json.i"
	@echo "... src/utils/json/json.s"
	@echo "... test/cpu/generater_test.o"
	@echo "... test/cpu/generater_test.i"
	@echo "... test/cpu/generater_test.s"
	@echo "... test/cpu/neural_test.o"
	@echo "... test/cpu/neural_test.i"
	@echo "... test/cpu/neural_test.s"
	@echo "... test/cpu/read_write_test.o"
	@echo "... test/cpu/read_write_test.i"
	@echo "... test/cpu/read_write_test.s"
	@echo "... test/cpu/snn_from_file.o"
	@echo "... test/cpu/snn_from_file.i"
	@echo "... test/cpu/snn_from_file.s"
	@echo "... test/cpu/synapse_test.o"
	@echo "... test/cpu/synapse_test.i"
	@echo "... test/cpu/synapse_test.s"
	@echo "... test/gpu/gpusim_test.o"
	@echo "... test/gpu/gpusim_test.i"
	@echo "... test/gpu/gpusim_test.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system


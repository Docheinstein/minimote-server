# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /home/stefano/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/192.7142.39/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/stefano/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/192.7142.39/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/stefano/Develop/C/MinimoteServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/stefano/Develop/C/MinimoteServer/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/MinimoteServer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MinimoteServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MinimoteServer.dir/flags.make

CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.o: ../src/adt/hash/hash.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/adt/hash/hash.c

CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/adt/hash/hash.c > CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.i

CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/adt/hash/hash.c -o CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.s

CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.o: ../src/adt/hash/node/hash_node.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/adt/hash/node/hash_node.c

CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/adt/hash/node/hash_node.c > CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.i

CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/adt/hash/node/hash_node.c -o CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.s

CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.o: ../src/adt/list/list.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/adt/list/list.c

CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/adt/list/list.c > CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.i

CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/adt/list/list.c -o CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.s

CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.o: ../src/adt/list/node/list_node.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/adt/list/node/list_node.c

CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/adt/list/node/list_node.c > CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.i

CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/adt/list/node/list_node.c -o CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.s

CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.o: ../src/arguments/arguments.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/arguments/arguments.c

CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/arguments/arguments.c > CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.i

CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/arguments/arguments.c -o CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.s

CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.o: ../src/buffer/rolling_buffer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/buffer/rolling_buffer.c

CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/buffer/rolling_buffer.c > CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.i

CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/buffer/rolling_buffer.c -o CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.s

CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.o: ../src/commons/utils/byte_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/commons/utils/byte_utils.c

CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/commons/utils/byte_utils.c > CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.i

CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/commons/utils/byte_utils.c -o CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.s

CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.o: ../src/commons/utils/net_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/commons/utils/net_utils.c

CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/commons/utils/net_utils.c > CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.i

CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/commons/utils/net_utils.c -o CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.s

CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.o: ../src/commons/utils/time_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/commons/utils/time_utils.c

CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/commons/utils/time_utils.c > CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.i

CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/commons/utils/time_utils.c -o CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.s

CMakeFiles/MinimoteServer.dir/src/main.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/main.c.o: ../src/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/MinimoteServer.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/main.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/main.c

CMakeFiles/MinimoteServer.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/main.c > CMakeFiles/MinimoteServer.dir/src/main.c.i

CMakeFiles/MinimoteServer.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/main.c -o CMakeFiles/MinimoteServer.dir/src/main.c.s

CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.o: ../src/minimote/client/minimote_client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/minimote/client/minimote_client.c

CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/minimote/client/minimote_client.c > CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.i

CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/minimote/client/minimote_client.c -o CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.s

CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.o: ../src/minimote/controller/minimote_controller.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/minimote/controller/minimote_controller.c

CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/minimote/controller/minimote_controller.c > CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.i

CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/minimote/controller/minimote_controller.c -o CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.s

CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.o: ../src/minimote/keys/minimote_key_type_x11_map.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/minimote/keys/minimote_key_type_x11_map.c

CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/minimote/keys/minimote_key_type_x11_map.c > CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.i

CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/minimote/keys/minimote_key_type_x11_map.c -o CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.s

CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.o: ../src/minimote/packet/minimote_packet.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building C object CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/minimote/packet/minimote_packet.c

CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/minimote/packet/minimote_packet.c > CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.i

CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/minimote/packet/minimote_packet.c -o CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.s

CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.o: ../src/minimote/packet/type/minimote_packet_type.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building C object CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/minimote/packet/type/minimote_packet_type.c

CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/minimote/packet/type/minimote_packet_type.c > CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.i

CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/minimote/packet/type/minimote_packet_type.c -o CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.s

CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.o: ../src/minimote/server/minimote_server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building C object CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/minimote/server/minimote_server.c

CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/minimote/server/minimote_server.c > CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.i

CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/minimote/server/minimote_server.c -o CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.s

CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.o: CMakeFiles/MinimoteServer.dir/flags.make
CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.o: ../src/minimote/x11/minimote_x11.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building C object CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.o   -c /home/stefano/Develop/C/MinimoteServer/src/minimote/x11/minimote_x11.c

CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stefano/Develop/C/MinimoteServer/src/minimote/x11/minimote_x11.c > CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.i

CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stefano/Develop/C/MinimoteServer/src/minimote/x11/minimote_x11.c -o CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.s

# Object files for target MinimoteServer
MinimoteServer_OBJECTS = \
"CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.o" \
"CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.o" \
"CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.o" \
"CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.o" \
"CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.o" \
"CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.o" \
"CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.o" \
"CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.o" \
"CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.o" \
"CMakeFiles/MinimoteServer.dir/src/main.c.o" \
"CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.o" \
"CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.o" \
"CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.o" \
"CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.o" \
"CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.o" \
"CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.o" \
"CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.o"

# External object files for target MinimoteServer
MinimoteServer_EXTERNAL_OBJECTS =

MinimoteServer: CMakeFiles/MinimoteServer.dir/src/adt/hash/hash.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/adt/hash/node/hash_node.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/adt/list/list.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/adt/list/node/list_node.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/arguments/arguments.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/buffer/rolling_buffer.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/commons/utils/byte_utils.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/commons/utils/net_utils.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/commons/utils/time_utils.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/main.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/minimote/client/minimote_client.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/minimote/controller/minimote_controller.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/minimote/keys/minimote_key_type_x11_map.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/minimote/packet/minimote_packet.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/minimote/packet/type/minimote_packet_type.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/minimote/server/minimote_server.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/src/minimote/x11/minimote_x11.c.o
MinimoteServer: CMakeFiles/MinimoteServer.dir/build.make
MinimoteServer: CMakeFiles/MinimoteServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Linking C executable MinimoteServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MinimoteServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MinimoteServer.dir/build: MinimoteServer

.PHONY : CMakeFiles/MinimoteServer.dir/build

CMakeFiles/MinimoteServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MinimoteServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MinimoteServer.dir/clean

CMakeFiles/MinimoteServer.dir/depend:
	cd /home/stefano/Develop/C/MinimoteServer/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/stefano/Develop/C/MinimoteServer /home/stefano/Develop/C/MinimoteServer /home/stefano/Develop/C/MinimoteServer/cmake-build-debug /home/stefano/Develop/C/MinimoteServer/cmake-build-debug /home/stefano/Develop/C/MinimoteServer/cmake-build-debug/CMakeFiles/MinimoteServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MinimoteServer.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jour/RpcChatserver-master/client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jour/RpcChatserver-master/client/src

# Include any dependencies generated for this target.
include src/CMakeFiles/client.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/client.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/client.dir/flags.make

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.o: src/CMakeFiles/client.dir/flags.make
src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.o: /home/jour/RpcChatserver-master/protos/ChatService.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jour/RpcChatserver-master/client/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.o"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.o -c /home/jour/RpcChatserver-master/protos/ChatService.pb.cc

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.i"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jour/RpcChatserver-master/protos/ChatService.pb.cc > CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.i

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.s"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jour/RpcChatserver-master/protos/ChatService.pb.cc -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.s

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.o: src/CMakeFiles/client.dir/flags.make
src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.o: /home/jour/RpcChatserver-master/protos/FriendService.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jour/RpcChatserver-master/client/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.o"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.o -c /home/jour/RpcChatserver-master/protos/FriendService.pb.cc

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.i"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jour/RpcChatserver-master/protos/FriendService.pb.cc > CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.i

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.s"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jour/RpcChatserver-master/protos/FriendService.pb.cc -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.s

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.o: src/CMakeFiles/client.dir/flags.make
src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.o: /home/jour/RpcChatserver-master/protos/GateServer.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jour/RpcChatserver-master/client/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.o"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.o -c /home/jour/RpcChatserver-master/protos/GateServer.pb.cc

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.i"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jour/RpcChatserver-master/protos/GateServer.pb.cc > CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.i

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.s"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jour/RpcChatserver-master/protos/GateServer.pb.cc -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.s

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.o: src/CMakeFiles/client.dir/flags.make
src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.o: /home/jour/RpcChatserver-master/protos/GroupService.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jour/RpcChatserver-master/client/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.o"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.o -c /home/jour/RpcChatserver-master/protos/GroupService.pb.cc

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.i"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jour/RpcChatserver-master/protos/GroupService.pb.cc > CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.i

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.s"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jour/RpcChatserver-master/protos/GroupService.pb.cc -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.s

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.o: src/CMakeFiles/client.dir/flags.make
src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.o: /home/jour/RpcChatserver-master/protos/OfflineService.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jour/RpcChatserver-master/client/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.o"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.o -c /home/jour/RpcChatserver-master/protos/OfflineService.pb.cc

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.i"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jour/RpcChatserver-master/protos/OfflineService.pb.cc > CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.i

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.s"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jour/RpcChatserver-master/protos/OfflineService.pb.cc -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.s

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.o: src/CMakeFiles/client.dir/flags.make
src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.o: /home/jour/RpcChatserver-master/protos/UserService.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jour/RpcChatserver-master/client/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.o"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.o -c /home/jour/RpcChatserver-master/protos/UserService.pb.cc

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.i"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jour/RpcChatserver-master/protos/UserService.pb.cc > CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.i

src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.s"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jour/RpcChatserver-master/protos/UserService.pb.cc -o CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.s

src/CMakeFiles/client.dir/client.cpp.o: src/CMakeFiles/client.dir/flags.make
src/CMakeFiles/client.dir/client.cpp.o: client.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jour/RpcChatserver-master/client/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object src/CMakeFiles/client.dir/client.cpp.o"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client.dir/client.cpp.o -c /home/jour/RpcChatserver-master/client/src/client.cpp

src/CMakeFiles/client.dir/client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/client.cpp.i"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jour/RpcChatserver-master/client/src/client.cpp > CMakeFiles/client.dir/client.cpp.i

src/CMakeFiles/client.dir/client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/client.cpp.s"
	cd /home/jour/RpcChatserver-master/client/src/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jour/RpcChatserver-master/client/src/client.cpp -o CMakeFiles/client.dir/client.cpp.s

# Object files for target client
client_OBJECTS = \
"CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.o" \
"CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.o" \
"CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.o" \
"CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.o" \
"CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.o" \
"CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.o" \
"CMakeFiles/client.dir/client.cpp.o"

# External object files for target client
client_EXTERNAL_OBJECTS =

../bin/client: src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/ChatService.pb.cc.o
../bin/client: src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/FriendService.pb.cc.o
../bin/client: src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GateServer.pb.cc.o
../bin/client: src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/GroupService.pb.cc.o
../bin/client: src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/OfflineService.pb.cc.o
../bin/client: src/CMakeFiles/client.dir/home/jour/RpcChatserver-master/protos/UserService.pb.cc.o
../bin/client: src/CMakeFiles/client.dir/client.cpp.o
../bin/client: src/CMakeFiles/client.dir/build.make
../bin/client: src/CMakeFiles/client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jour/RpcChatserver-master/client/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX executable ../../bin/client"
	cd /home/jour/RpcChatserver-master/client/src/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/client.dir/build: ../bin/client

.PHONY : src/CMakeFiles/client.dir/build

src/CMakeFiles/client.dir/clean:
	cd /home/jour/RpcChatserver-master/client/src/src && $(CMAKE_COMMAND) -P CMakeFiles/client.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/client.dir/clean

src/CMakeFiles/client.dir/depend:
	cd /home/jour/RpcChatserver-master/client/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jour/RpcChatserver-master/client /home/jour/RpcChatserver-master/client/src /home/jour/RpcChatserver-master/client/src /home/jour/RpcChatserver-master/client/src/src /home/jour/RpcChatserver-master/client/src/src/CMakeFiles/client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/client.dir/depend


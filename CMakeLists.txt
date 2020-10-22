# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS -pthread)

project ("ChatServerClient")

add_executable (ChatServer 
"ChatServerClient/ChatServer.cpp" 
"ChatServerClient/ChatServer.h")

add_executable (ChatClient 
"ChatServerClient/ChatClient.cpp" 
"ChatServerClient/ChatClient.h")

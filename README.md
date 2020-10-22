# ChatServerClient
Dead simple "chat" system TCP written in C++
Server run on address "127.0.0.1" on port "54000"

# Project architecture

project  
│   README.md  
│   CMakeLists.txt  
│   .gitignore  
│   
└───ChatServerClient  
──────│ ChatClient.cpp  
──────│ ChatClient.h  
──────│ ChatServer.cpp  
──────│ ChatServer.h  

# Ubuntu

Install cmake and compiler  
`sudo apt  install cmake`  
`sudo apt-get update && sudo apt-get install build-essential`  

Generate executables. You need to be in parent folder  
`cmake .`  
`make`  

After that, it's generate 2 executable files named `ChatServer` to launch server and `ChatClient` to launch client
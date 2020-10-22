#pragma once

#include <thread>
#include <iostream>
#include <string>
#include <cstring>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WS2tcpip.h>
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

#else

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using SOCKET = int;
#define SD_BOTH SHUT_RDWR
#define closesocket(s) close(s)
#endif
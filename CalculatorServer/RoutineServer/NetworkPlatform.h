#pragma once
#pragma once

// 플랫폼 감지 매크로
#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#elif defined(__linux__)
#define PLATFORM_LINUX
#elif defined(__APPLE__)
#define PLATFORM_MAC
#else
#error "지원하지 않는 플랫폼입니다"
#endif

// Windows 플랫폼 설정
#ifdef PLATFORM_WINDOWS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

// Windows 라이브러리 자동 링크
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wsock32.lib")

// Windows 전용 타입 정의
typedef SOCKET SocketType;
typedef int socklen_t;
typedef SOCKADDR_IN SockAddrIn;
typedef SOCKADDR SockAddr;

// Windows 전용 상수
#define INVALID_SOCKET_VALUE INVALID_SOCKET
#define SOCKET_ERROR_VALUE SOCKET_ERROR
#define CLOSE_SOCKET(s) closesocket(s)
#define GET_LAST_ERROR() WSAGetLastError()

// wsaData를 매개변수로 받는 매크로로 수정
#define NETWORK_INIT(wsaData) WSAStartup(MAKEWORD(2, 2), &wsaData)
#define NETWORK_CLEANUP() WSACleanup()

#endif

// Linux 플랫폼 설정  
#ifdef PLATFORM_LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

// Linux 전용 타입 정의
typedef int SocketType;
typedef sockaddr_in SockAddrIn;
typedef sockaddr SockAddr;

// Linux 전용 상수
#define INVALID_SOCKET_VALUE -1
#define SOCKET_ERROR_VALUE -1
#define CLOSE_SOCKET(s) close(s)
#define GET_LAST_ERROR() errno

//  Linux는 wsaData 불필요하므로 매개변수 무시
#define NETWORK_INIT(wsaData) 0
#define NETWORK_CLEANUP() 

#endif

// 나머지 매크로들은 동일...
#define MAX_BUFFER_SIZE 1024

#ifdef PLATFORM_WINDOWS
#define SET_SOCKET_TIMEOUT(socket, timeout_ms) \
        do { \
            DWORD timeout = timeout_ms; \
            setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)); \
            setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)); \
        } while(0)
#else
#define SET_SOCKET_TIMEOUT(socket, timeout_ms) \
        do { \
            struct timeval timeout; \
            timeout.tv_sec = timeout_ms / 1000; \
            timeout.tv_usec = (timeout_ms % 1000) * 1000; \
            setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); \
            setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)); \
        } while(0)
#endif
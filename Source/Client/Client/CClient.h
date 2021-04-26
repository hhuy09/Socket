#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <tchar.h>
#include <WS2tcpip.h>
#include <string.h>
#include<string>
#include<iostream>
#include<conio.h>
#include<fcntl.h>
#include<Windows.h>
#include<thread>
#include <mutex>
using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

class CClient
{
private:
	WORD wVersionRequested;
	WSADATA wsaData;
	int retcode;
	SOCKET socket_descriptor;
	struct sockaddr_in sin;
	int addrlen;

public:
	CClient();
	~CClient();
	void _WSAStartup();
	void createSocket();
	void closeSocket();
	bool _connect();
	SOCKET& getMainSocket();

};
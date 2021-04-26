#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib, "Ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <tchar.h>
#include<fstream>
#include <WS2tcpip.h>
#include<string>
#include<iostream>
#include<vector>
#include<mutex>
#include<thread>
using namespace std;

class CServer
{
private:
	WORD wVersionRequested;
	WSADATA wsaData;
	int retcode;
	SOCKET socket_descriptor;
	struct sockaddr_in sin;
	int addrlen;

public:
	//Đây là một số hàm để tự khởi tạo Server
	CServer();
	~CServer();
	void _WSAStartup();
	void createSocket();
	void closeSocket();
	SOCKET& getMainSocket();

};

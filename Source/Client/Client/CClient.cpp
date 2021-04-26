#include "CClient.h"

void errexit(const char* format, ...)
{
	va_list	args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	WSACleanup();
	exit(1);
}

CClient::CClient()
{
	_WSAStartup();
	createSocket();
}

CClient::~CClient()
{

}

void CClient::_WSAStartup()
{
	printf("WSAStartup()\n");
	wVersionRequested = MAKEWORD(2, 2);	// Use MAKEWORD(1,1) if you're at WinSock 1.1
	retcode = WSAStartup(wVersionRequested, &wsaData);
	if (retcode != 0)
		errexit("Startup failed: %d\n", retcode);

	if (LOBYTE(wsaData.wVersion) != LOBYTE(wVersionRequested) ||
		HIBYTE(wsaData.wVersion) != HIBYTE(wVersionRequested))
	{
		printf("Supported version is too low\n");
		WSACleanup();
		return;
	}
}

void CClient::createSocket()
{
	printf("socket()\n");
	socket_descriptor = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_descriptor == INVALID_SOCKET)
		errexit("Socket creation failed: %d\n", WSAGetLastError());


	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4984);
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");

}

void CClient::closeSocket()
{
	printf("closesocket()\n");
	retcode = closesocket(socket_descriptor);
	if (retcode == SOCKET_ERROR)
		errexit("Close socket failed: %d\n", WSAGetLastError());

	printf("Return Code: %d\n", retcode);
	printf("\n");
	printf("closesocket()\n");
}

bool  CClient::_connect()
{
	printf("connect()\n");
	retcode = connect(socket_descriptor, (struct sockaddr*)&sin, sizeof(sin));
	if (retcode == SOCKET_ERROR)
	{
		errexit("Connect failed: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

SOCKET& CClient::getMainSocket()
{
	return socket_descriptor;
}

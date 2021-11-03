#include "Protocol.h"

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

void Socket::Bind(short ServerPort)
{
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	int retval = bind(s_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		Exception(retval);
}

void Socket::Listen() 
{
	s_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (s_socket == INVALID_SOCKET)
		Exception(s_socket);
}

void Socket::Connect(const char* ServerAddress, short ServerPort)
{
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serveraddr.sin_port = htons(SERVER_PORT);
	int retval = connect(s_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		Exception(s_socket);
}

SOCKET Socket::Accept()
{
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	c_socket = accept(s_socket, (SOCKADDR*)&clientaddr, &addrlen);
	if (c_socket == INVALID_SOCKET)
		Exception(c_socket);
}

void Socket::Send(Message& msg) 
{
	int size = sizeof(msg);
	int retval = send(c_socket, (char*)&size, sizeof(size), 0);
	if (retval == SOCKET_ERROR)
		Exception(retval);

	retval = send(c_socket, (char*)&msg, sizeof(msg), 0);
	if (retval == SOCKET_ERROR)
		Exception(retval);
}

void Socket::Recv()
{
	int len;
	int retval = recvn(c_socket, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
		Exception(retval);

	retval = recvn(c_socket, (char*)&recvMessage, sizeof(recvMessage), 0);
	if (retval == SOCKET_ERROR)
		Exception(retval);
}

int main()
{
	
}
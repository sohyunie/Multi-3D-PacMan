#include "Exception.h"
#include "Socket.h"

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


Socket::Socket()
{
}

Socket::~Socket()
{
	if (m_socket)
		closesocket(m_socket);
}

void Socket::Bind(short ServerPort)
{
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	int retval = bind(m_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		throw Exception(WSAGetLastError());
}

void Socket::Listen() 
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
		throw Exception(WSAGetLastError());
}

void Socket::Connect(const char* ServerAddress, short ServerPort)
{
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, ServerAddress, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVER_PORT);
	int retval = connect(m_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		throw Exception(WSAGetLastError());
}

SOCKET Socket::Accept()
{
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	SOCKET accept_socket = accept(m_socket, (SOCKADDR*)&clientaddr, &addrlen);
	if (accept_socket == INVALID_SOCKET)
		throw Exception(WSAGetLastError());
	return accept_socket;
}

void Socket::Send(Message& msg) 
{
	int size = sizeof(msg);
	int retval = send(m_socket, (char*)&size, sizeof(size), 0);
	if (retval == SOCKET_ERROR)
		throw Exception(WSAGetLastError());

	retval = send(m_socket, (char*)&msg, sizeof(msg), 0);
	if (retval == SOCKET_ERROR)
		throw Exception(WSAGetLastError());
}

void Socket::Recv()
{
	int len;
	int retval = recvn(m_socket, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
		throw Exception(WSAGetLastError());

	retval = recvn(m_socket, m_recvMessage.MsgBuffer, sizeof(m_recvMessage.MsgBuffer), 0);
	if (retval == SOCKET_ERROR)
		throw Exception(WSAGetLastError());
}
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
	: m_socket(), m_recvBuffer{}
{
}

Socket::~Socket()
{
	if (m_socket)
		closesocket(m_socket);
}

void Socket::Init()
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
		throw Exception("socket failed");
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
		throw Exception("bind failed");
}

void Socket::Listen() 
{
	if (listen(m_socket, 50) != 0)
		throw Exception("listen failed");
}

void Socket::Connect(const char* const ServerAddress, short ServerPort)
{
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, ServerAddress, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(ServerPort);
	int retval = connect(m_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		throw Exception("connect failed");
}

SOCKET Socket::Accept()
{
	SOCKET accept_socket = accept(m_socket, NULL, 0);
	if (accept_socket == INVALID_SOCKET)
		throw Exception("accept failed");
	return accept_socket;
}

int Socket::Send(Message& msg) 
{
	short size = msg.GetTotalMsgSize();
	
	int retval = send(m_socket, (char*)&size, sizeof(short), 0);
	if (retval == SOCKET_ERROR)
		throw Exception("send size failed");
	if (retval == 0)
		return retval;

	retval = send(m_socket, (char*)&msg.m_buffer, size, 0);
	if (retval == SOCKET_ERROR)
		throw Exception("send buffer failed");

	return retval;
}

int Socket::Recv()
{
	int len = 0;
	int retval = recvn(m_socket, (char*)&len, sizeof(short), 0);
	if (retval == SOCKET_ERROR) 
		throw Exception("recv failed");
	if (retval == 0)
		return retval;

	retval = recvn(m_socket, m_recvBuffer, len, 0);
	if (retval == SOCKET_ERROR)
		throw Exception("recv failed");

	if(retval != 0)
	{ 
		m_recvMsg.Push(m_recvBuffer, len);
	}
	return retval;
}

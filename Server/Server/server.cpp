#include "server.h"

Server::Server()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		throw Exception(WSAGetLastError());
}

Server::~Server()
{
	WSACleanup();
}

void Server::Update()
{
	for (;;)
	{
		cout << "Hello\n" << endl;
	}
}
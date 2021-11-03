#pragma once

#include "clientInfo.h"

class Server
{
public:
	Server();
	~Server();

	void Update();

public:
	static mutex g_mapInfoLock;
	static mutex g_countOfKeyLock;
	static MapInfo map;
	static int countOfKeyAccquired;

private:
	WSAData m_wsaData;
	Socket m_listenSock;
	array<ClientInfo, 3> m_clients;
	array<std::thread, 3> m_threads;
};
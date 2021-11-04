#pragma once

#include "clientInfo.h"

class Server
{
public:
	Server();
	~Server();

	void Update();
	void LoadMap(const char* filename);
	void AcceptNewPlayer(int id);
	
	static void RecvAndSend(int id);
	
	void CreatePlayerJoinMsg();
	void CreateStartGameMsg();
	void CreateUpdateMapInfoMsg();

public:
	static mutex g_mapInfoLock;
	static mutex g_countOfKeyLock;
	static MapInfo map;
	static int countOfKeyAccquired;

	static array<ClientInfo, 3> m_clients;

private:
	WSAData m_wsaData;
	Socket m_listenSock;
	array<std::thread, 3> m_threads;
};
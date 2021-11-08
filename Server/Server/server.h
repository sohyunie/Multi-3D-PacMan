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

	static vector<ClientInfo> m_clients;
	static const int MaxClients = 1;

private:
	WSAData m_wsaData;
	Socket m_listenSock;
	vector<std::thread> m_threads;

	start_game startGameData;
};
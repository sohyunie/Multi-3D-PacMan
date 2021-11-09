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
<<<<<<< HEAD
	static const int MaxClients = 3;
=======
	static const int MaxClients = 1;
>>>>>>> main

private:
	WSAData m_wsaData;
	Socket m_listenSock;
	vector<std::thread> m_threads;
<<<<<<< HEAD
=======

	start_game startGameData;
>>>>>>> main
};
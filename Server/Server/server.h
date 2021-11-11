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
	
	static void SendAndRecv(int id);
	
	void CreatePlayerJoinMsg();
	void CreateStartGameMsg();
	void CreateUpdateMapInfoMsg();

	void GameStart();

public:
	static mutex g_mapInfoLock;
	static mutex g_countOfKeyLock;
	static MapInfo map;
	static int countOfKeyAccquired;

	static vector<ClientInfo> g_clients;
	static const int MaxClients = 3;
	static bool g_loop;

	static Timer g_timer;
	static float g_accum_time;

private:
	WSAData m_wsaData;
	Socket m_listenSock;
	vector<std::thread> m_threads;

	start_game startGameData;
};
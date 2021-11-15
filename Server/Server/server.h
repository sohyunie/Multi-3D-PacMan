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
	void InitializeStartGameInfo();
	void CreateUpdateMapInfoMsg();

	void GameStart();

public:
	static mutex g_mapInfoLock;
	static mutex g_countOfKeyLock;
	static MapInfo g_map;
	static int countOfKeyAccquired;

	static vector<ClientInfo> g_clients;
	static const int MaxClients = 3;

	static mutex g_timerLock;
	static condition_variable g_timerCv;
	static Timer g_timer;
	static float g_accum_time;

	static mutex g_loopLock;
	static condition_variable g_loopCv;
	static bool g_loop;

private:
	WSAData m_wsaData;
	Socket m_listenSock;
	vector<thread> m_threads;

	start_game startGameData;
};
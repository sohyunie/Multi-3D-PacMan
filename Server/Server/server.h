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
	
	void GameStart();
	void InitializeStartGameInfo();

	static void SendAndRecv(int id);
	
	void CreatePlayerJoinMsg();
	void CreatePlayerInfoMsg(float elapsedTime);
	void CreateUpdateStatusMsg();

	vector<object_status> UpdateObjectStatus(int id);
	bool CheckWinStatus(int id);
	bool IsCollided(const Vector4& a, const Vector4& b);	

public:
	static mutex g_mapInfoLock;
	static MapInfo g_map;

	static const int MaxClients = 3;
	static array<ClientInfo, MaxClients> g_clients;

	static mutex g_timerLock;
	static condition_variable g_timerCv;
	static Timer g_timer;
	static float g_accum_time;

	static mutex g_loopLock;
	static condition_variable g_loopCv;
	static bool g_loop;

	static mutex g_sendMsgLock;
	static Message g_sendMsg;

private:
	WSAData m_wsaData;
	Socket m_listenSock;
	vector<thread> m_threads;

	start_game m_startGameData;
	int m_countOfKeyAccquired = 0;
};
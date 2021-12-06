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

	void CopySendMsgToAllClients();

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

private:
	WSAData m_wsaData;
	Socket m_listenSock;
	vector<thread> m_threads;

	update_player_info m_player_info;
	update_status m_update_info;
	vector<object_status> m_object_info;

	start_game m_startGameData;
	int m_countOfKeyAccquired = 0;

	int m_taggerIndex = 0;
	
	const float m_offset = 7.5f;
	const int maxClient = 3; // TEST
};
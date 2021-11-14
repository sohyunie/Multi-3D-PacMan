#include "server.h"

mutex Server::g_mapInfoLock;
mutex Server::g_countOfKeyLock;
MapInfo Server::map;
int Server::countOfKeyAccquired = 0;

mutex Server::g_timerLock;
condition_variable Server::g_timerCv;
Timer Server::g_timer;
float Server::g_accum_time = 0.0f;

mutex Server::g_loopLock;
condition_variable Server::g_loopCv;
bool Server::g_loop = false;

vector<ClientInfo> Server::g_clients;


Server::Server()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		throw Exception("WSAStartup failed");

	LoadMap("map.txt");

	m_listenSock.Init();
	m_listenSock.Bind(SERVER_PORT);
	m_listenSock.Listen();

	cout << "Listening to clients..\n";
}

Server::~Server()
{
	for (thread& thrd : m_threads)
		thrd.join();

	WSACleanup();
}

void Server::Update()
{
	int max_clients = 1; // temporary variable
	for (int i = 0; i < max_clients; i++)
		AcceptNewPlayer(i);

	GameStart();
	std::cout << "Game Start\n";
	g_timer.Start();

	while(g_loop)
	{
		g_timer.Tick();
		g_accum_time += g_timer.GetElapsedTime();
		if (g_accum_time >= 1.0f) 
		{
			g_timerCv.notify_all();
			g_accum_time = 0.0f;
		}

		// do other things..

	}
}

void Server::LoadMap(const char* filename)
{
	ifstream in(filename);
	ObjectInfo object;

	int bead_id = 0, key_id = 0, player_id = 0;
	char mapn;
	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 30; ++j) {
			in >> mapn;
			startGameData.mapinfo[i][j] = mapn;	// 게임 시작시 보낼 맵 정보 저장
			
			object.active = true;
			object.x = (float)j;
			object.z = (float)i;
			object.boundingOffset = 1.0;

			if (mapn == 0) {					// BEAD
				object.id = bead_id++;
				object.type = ObjectType::BEAD;
				map.beads.push_back(object);
			}
			else if (mapn == 1) {			// KEY
				object.id = key_id++;
				object.type = ObjectType::KEY;
				map.keys.push_back(object);
			}
			else if (mapn == 2) {			// WALL
				object.id = 0;
				object.type = ObjectType::WALL;					
				map.walls.push_back(object);
			}
			else if (mapn == 3) {			// PLAYER_POS
				startGameData.x[player_id] = (float)j;
				startGameData.z[player_id++] = (float)i;
			}
			else if (mapn == 4) {			// DOOR
				map.door.active = true;
				map.door.x = (float)j;
				map.door.z = (float)i;
				map.door.id = 0;
				map.door.type = ObjectType::DOOR;
				map.door.boundingOffset = 1.0;
			}
		}
	}
}

void Server::AcceptNewPlayer(int id)
{
	SOCKET new_client = m_listenSock.Accept();
	g_clients.emplace_back(new_client, id);
	m_threads.emplace_back(SendAndRecv, id);
	std::cout << "Accepted New Client[" << id << "]\n";
}

void Server::SendAndRecv(int id)
{
	try {
		{
			unique_lock<mutex> loop_lock(g_loopLock);
			g_loopCv.wait(loop_lock, [](){ return g_loop; });
		}

		while (g_loop)
		{
			{
				unique_lock<mutex> timer_lock(g_timerLock);
				g_timerCv.wait(timer_lock);
			}
			std::cout << "working..[" << id << "]\n";
		}
	}
	catch (Exception& ex)
	{
		cout << "[" << id << "] " << ex.what() << endl;
		return;
	}	
}

void Server::CreatePlayerJoinMsg()
{
}

void Server::CreateStartGameMsg()
{
	startGameData.type = MsgType::START_GAME;
	startGameData.size = sizeof(startGameData);
	for (int i = 0; i < MaxClients; ++i)
		startGameData.id[i] = (char)i;
	startGameData.playertype[0] = PlayerType::RUNNER;
	startGameData.playertype[1] = PlayerType::TAGGER;
	startGameData.playertype[2] = PlayerType::RUNNER;
}

void Server::CreateUpdateMapInfoMsg()
{

}

void Server::GameStart()
{
	CreateStartGameMsg();

	for (int i = 0; i < g_clients.size(); i++)
	{
		startGameData.my_id = i;
		g_clients[i].CreateLoginOkAndMapInfoMsg(startGameData);
	}

	g_loop = true;
	g_loopCv.notify_all();
}

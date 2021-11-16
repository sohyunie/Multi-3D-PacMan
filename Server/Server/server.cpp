#include "server.h"

mutex Server::g_mapInfoLock;
MapInfo Server::g_map;

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
				g_map.beads.push_back(object);
			}
			else if (mapn == 1) {			// KEY
				object.id = key_id++;
				object.type = ObjectType::KEY;
				g_map.keys.push_back(object);
			}
			else if (mapn == 2) {			// WALL
				object.id = 0;
				object.type = ObjectType::WALL;					
				g_map.walls.push_back(object);
			}
			else if (mapn == 3) {			// PLAYER_POS
				startGameData.x[player_id] = (float)j;
				startGameData.z[player_id++] = (float)i;
			}
			else if (mapn == 4) {			// DOOR
				g_map.door.active = true;
				g_map.door.x = (float)j;
				g_map.door.z = (float)i;
				g_map.door.id = 0;
				g_map.door.type = ObjectType::DOOR;
				g_map.door.boundingOffset = 1.0;
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
			g_clients[id].Send();
			g_clients[id].Recv();
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

void Server::InitializeStartGameInfo()
{
	startGameData.type = MsgType::START_GAME;
	startGameData.size = sizeof(startGameData);
	for (int i = 0; i < MaxClients; ++i) {
		startGameData.id[i] = (char)i;
		startGameData.playertype[i] = PlayerType::RUNNER;

		g_clients[i].m_id = i;
		g_clients[i].m_type = PlayerType::RUNNER;
		g_clients[i].m_hp = 100;
		g_clients[i].m_pos_x = startGameData.x[i];
		g_clients[i].m_pos_z = startGameData.z[i];
		g_clients[i].m_boundingOffset = 1.5f;
	}

	int tagger = rand() % MaxClients;
	g_clients[tagger].m_type = PlayerType::TAGGER;
}

void Server::CreateUpdateMapInfoMsg()
{
	update_status update_stat{};
	update_stat.win = WinStatus::NONE;

	vector<object_status> all_obj_status;
	for (int i=0;i<g_clients.size();i++)
	{
		vector<object_status> stats = UpdateObjectStatus(i);
		all_obj_status.insert(all_obj_status.end(), stats.begin(), stats.end());

		if (CheckWinStatus(i))
			update_stat.win = WinStatus::RUNNER_WIN;
	}
	
	// TODO: 적과의 충돌 후 hp 감소..
	// TODO: 모든 플레이어의 사망여부 체크
	
	// TODO: 모든 플레이어에게 메시지 보내기

}

vector<object_status> Server::UpdateObjectStatus(int id)
{
	vector<object_status> obj_stats;

	const Vector4& clientBB = g_clients[id].GetBoundingBox();

	g_mapInfoLock.lock();
	for (ObjectInfo& bead : g_map.beads)
	{
		if (bead.active)
		{
			const Vector4& beadBB = bead.GetBoundingBox();
			if (IsCollided(clientBB, beadBB))
			{
				bead.active = false;
				obj_stats.emplace_back(bead.type, bead.id, bead.active);
			}
		}
	}
	for (ObjectInfo& key : g_map.keys)
	{
		if (key.active)
		{
			const Vector4& keyBB = key.GetBoundingBox();
			if (IsCollided(clientBB, keyBB))
			{
				key.active = false;
				m_countOfKeyAccquired += 1;
				obj_stats.emplace_back(key.type, key.id, key.active);
			}
		}
	}
	g_mapInfoLock.unlock();
	
	return obj_stats;
}

bool Server::CheckWinStatus(int id)
{
	if (m_countOfKeyAccquired >= 3)
	{
		const Vector4& clientBB = g_clients[id].GetBoundingBox();
		const Vector4& doorBB = g_map.door.GetBoundingBox();
		if (IsCollided(clientBB, doorBB))
		{
			return true;
		}
	}
	return false;
}

bool Server::IsCollided(const Vector4& a, const Vector4& b)
{
	if (a.MinX > b.MaxX || b.MinX > a.MinX)
		return false;

	if (a.MinZ > b.MaxZ || b.MinZ > a.MaxZ)
		return false;

	return true;
}

void Server::GameStart()
{
	InitializeStartGameInfo();

	for (int i = 0; i < g_clients.size(); i++)
	{
		startGameData.my_id = i;
		g_clients[i].CreateLoginOkAndMapInfoMsg(startGameData);
	}

	g_loop = true;
	g_loopCv.notify_all();
}

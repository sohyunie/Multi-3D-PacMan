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

mutex Server::g_sendMsgLock; // TEST


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
		// calculate new position
		// check map status
		// check tagger and runner collision
		// check win status

		// TEST
		//update_player_info player_info{};
		//player_info.size = sizeof(update_player_info);
		//player_info.type = MsgType::UPDATE_PLAYER_INFO;
		//for (int i = 0; i < 3; i++) {
		//	player_info.x[i] = i;
		//	player_info.z[i] = i;
		//}
		//
		//for (auto& client : g_clients) {
		//	//g_sendMsgLock.lock();
		//	client.m_sendMsg.Clear();
		//	client.m_sendMsg.Push(reinterpret_cast<char*>(&player_info), sizeof(update_player_info));
		//	//cout << "[main] Pushing packets\n";
		//	//g_sendMsgLock.unlock();
		//}

		// TEST: 연산이 많으면 Tick의 빈도가 적어진다.
		// 타이머의 시간 계산은 정확하지만, 다른 스레드에 알리는 시간은 1초를 넘어선다.
		/*unsigned long long sum = 0;
		for (int i = 0; i < 1'000'000'000; i++)
			sum += i * 2;*/		
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

	in.close();
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
			std::cout << "[" << id << "] Tick!" << std::endl;
			//g_clients[id].Recv();
			//g_clients[id].ProcessMessage();

			//g_sendMsgLock.lock();
			//g_clients[id].Send();
			//g_sendMsgLock.unlock();
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

void Server::GameStart()
{
	InitializeStartGameInfo();

	for (int i = 0; i < g_clients.size(); i++)
	{
		startGameData.my_id = i;
		g_clients[i].m_sendMsg.Push(reinterpret_cast<char*>(&startGameData), sizeof(start_game));
		g_clients[i].Send();
	}

	g_loop = true;
	g_loopCv.notify_all();
}

void Server::InitializeStartGameInfo()
{
	startGameData.type = MsgType::START_GAME;
	startGameData.size = sizeof(startGameData);
	for (int i = 0; i < g_clients.size(); ++i) {
		startGameData.id[i] = (char)i;
		startGameData.playertype[i] = PlayerType::RUNNER;

		g_clients[i].m_id = i;
		g_clients[i].m_type = PlayerType::RUNNER;
		g_clients[i].m_hp = 100;
		g_clients[i].m_pos_x = startGameData.x[i];
		g_clients[i].m_pos_z = startGameData.z[i];
		g_clients[i].m_boundingOffset = 1.5f;
	}

	int tagger = rand() % g_clients.size();
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
				obj_stats.push_back({ bead.type , bead.id, bead.active });
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
				obj_stats.push_back({ key.type, key.id, key.active });
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

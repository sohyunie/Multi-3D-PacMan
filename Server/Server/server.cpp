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

array<ClientInfo, Server::MaxClients> Server::g_clients;

Server::Server()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		throw Exception("WSAStartup failed");

	LoadMap("map.txt");

	m_listenSock.Init();
	m_listenSock.Bind(SERVER_PORT);
	m_listenSock.Listen();

	m_threads.reserve(MaxClients);

	cout << "Listening to clients..\n";
}

Server::~Server()
{
	for (thread& thrd : m_threads)
		thrd.join();

	WSACleanup();
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
			m_startGameData.mapinfo[i][j] = mapn;	// 게임 시작시 보낼 맵 정보 저장

			object.active = true;
			object.row = (char)j;
			object.col = (char)i;
			object.x = i * m_offset;
			object.z = j * m_offset;
			object.boundingOffset = 1.0;

			if (mapn == '0') {
				object.type = ObjectType::BEAD;
				g_map.beads.push_back(object);
			}
			else if (mapn == '1') {
				object.type = ObjectType::KEY;
				g_map.keys.push_back(object);
			}
			else if (mapn == '2') {
				object.type = ObjectType::WALL;
				g_map.walls.push_back(object);
			}
			else if (mapn == '3') {			// PLAYER_POS
				m_startGameData.x[player_id] = (float)j;
				m_startGameData.z[player_id++] = (float)i;
			}
			else if (mapn == '4') {			// DOOR
				g_map.door.active = false;
				g_map.door.x = (char)j;
				g_map.door.z = (char)i;
				g_map.door.type = ObjectType::DOOR;
				g_map.door.boundingOffset = 1.0;
			}
		}
	}

	in.close();
}

void Server::Update()
{
	for (int i = 0; i < maxClient; i++)
		AcceptNewPlayer(i);

	GameStart();
	std::cout << "Game Start\n";
	g_timer.Start();

	while (g_loop)
	{
		g_timer.Tick();
		g_accum_time += g_timer.GetElapsedTime();
		if (g_accum_time >= 1.0f)
		{
			CopySendMsgToAllClients();
			g_timerCv.notify_all();
			g_accum_time = 0.0f;
		}
		CreatePlayerInfoMsg(g_timer.GetElapsedTime());
		CreateUpdateStatusMsg();
	}
}

void Server::AcceptNewPlayer(int id)
{
	SOCKET new_client = m_listenSock.Accept();
	g_clients[id].Init(new_client, id);
	m_threads.emplace_back(SendAndRecv, id);
	std::cout << "Accepted New Client[" << id << "]\n";
}

void Server::GameStart()
{
	InitializeStartGameInfo();

	for (int i = 0; i < maxClient; i++)
	{
		Message sendMsg{};
		m_startGameData.my_id = i;
		sendMsg.Push(reinterpret_cast<char*>(&m_startGameData), sizeof(start_game));
		g_clients[i].Send(sendMsg);
	}
	g_loop = true;
	g_loopCv.notify_all();
}

void Server::InitializeStartGameInfo()
{
	m_startGameData.type = MsgType::START_GAME;
	m_startGameData.size = sizeof(m_startGameData);
	for (int i = 0; i < g_clients.size(); ++i) {
		m_startGameData.id[i] = (char)i;
		m_startGameData.playertype[i] = PlayerType::RUNNER;

		g_clients[i].m_id = (char)i;
		g_clients[i].m_type = PlayerType::RUNNER;
		g_clients[i].m_hp = 100;
		g_clients[i].m_pos_x = m_startGameData.x[i];
		g_clients[i].m_pos_z = m_startGameData.z[i];
		g_clients[i].m_boundingOffset = 1.5f;
	}

	m_taggerIndex = rand() % g_clients.size();
	g_clients[m_taggerIndex].m_type = PlayerType::TAGGER;
	m_startGameData.playertype[m_taggerIndex] = PlayerType::TAGGER;
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
			//std::cout << "[" << id << "] Tick!" << std::endl;
			g_clients[id].Recv();
			g_clients[id].ProcessMessage();
			g_clients[id].SendMsg();
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

void Server::CreatePlayerInfoMsg(float elapsedTime)
{
	m_player_info.size = sizeof(update_player_info);
	m_player_info.type = MsgType::UPDATE_PLAYER_INFO;
	for (int i = 0; i < g_clients.size(); i++)
	{
		g_clients[i].SetNewPosition(m_startGameData, elapsedTime);
		m_player_info.id[i] = g_clients[i].m_id;
		m_player_info.x[i] = g_clients[i].m_pos_x;
		m_player_info.z[i] = g_clients[i].m_pos_z;
	}
}

void Server::CreateUpdateStatusMsg()
{
	m_update_info.type = MsgType::UPDATE_STATUS;
	m_update_info.win = WinStatus::NONE;

	int deathCount = 0;
	const Vector4& taggerBB = g_clients[m_taggerIndex].GetBoundingBox();
	for (int i = 0; i < g_clients.size(); i++)
	{
		if (g_clients[i].m_active == false) continue;

		vector<object_status> stats = UpdateObjectStatus(i);
		m_object_info.insert(m_object_info.end(), stats.begin(), stats.end());
		
		if (i == m_taggerIndex) continue;

		if (CheckWinStatus(i))
		{
			m_update_info.win = WinStatus::RUNNER_WIN;
			break;
		}

		const Vector4& runnerBB = g_clients[i].GetBoundingBox();
		if (IsCollided(taggerBB, runnerBB))
		{
			g_clients[i].m_hp -= 25;

			if (g_clients[i].m_hp <= 0) {
				g_clients[i].m_active = false;
				deathCount += 1;
			}
		}

		m_update_info.player_active[i] = g_clients[i].m_active;
	}

	if (deathCount >= MaxClients)
		m_update_info.win = WinStatus::TAGGER_WIN;

	m_update_info.size = sizeof(update_status) + m_object_info.size() * sizeof(object_status);
}

void Server::CopySendMsgToAllClients()
{
	Message sendMsg{};
	sendMsg.Push(reinterpret_cast<char*>(&m_player_info), sizeof(update_player_info));
	sendMsg.Push(reinterpret_cast<char*>(&m_update_info), sizeof(update_status));
	cout << "object size: " << m_object_info.size() << std::endl;
	sendMsg.Push(reinterpret_cast<char*>(m_object_info.data()), m_object_info.size() * sizeof(object_status));
	m_object_info.clear();

	for (ClientInfo& client : g_clients)
		client.m_sendMsg = sendMsg;
}

vector<object_status> Server::UpdateObjectStatus(int id)
{
	vector<object_status> obj_stats;

	const Vector4& clientBB = g_clients[id].GetBoundingBox();

	//g_mapInfoLock.lock();
	for (ObjectInfo& bead : g_map.beads)
	{
		if (bead.active)
		{
			const Vector4& beadBB = bead.GetBoundingBox();
			if (IsCollided(clientBB, beadBB))
			{
				bead.active = false;
				obj_stats.push_back({ bead.type , bead.row, bead.col, bead.active });
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
				obj_stats.push_back({ key.type, key.row, key.col, key.active });
			}
		}
	}
	if (m_countOfKeyAccquired >= 3) {
		g_map.door.active = true;
		obj_stats.push_back({ g_map.door.type, g_map.door.row, g_map.door.col, g_map.door.active });
	}
	//g_mapInfoLock.unlock();
	
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
	if (a.MinX > b.MaxX || b.MinX > a.MaxX)
		return false;

	if (a.MinZ > b.MaxZ || b.MinZ > a.MaxZ)
		return false;

	return true;
}

#include "server.h"

mutex Server::g_mapInfoLock;
mutex Server::g_countOfKeyLock;
MapInfo Server::map;
int Server::countOfKeyAccquired;

vector<ClientInfo> Server::m_clients;

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
	CreateStartGameMsg();

	for (int i = 0; i < MaxClients; i++)
		AcceptNewPlayer(i);

	while(true)
	{
		Sleep(500);
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
	m_clients.emplace_back(new_client, id);

	startGameData.my_id = id;
	m_clients.back().CreateLoginOkAndMapInfoMsg(startGameData);

	m_threads.emplace_back(RecvAndSend, id);
}

void Server::RecvAndSend(int id)
{
	bool loop = true;

	try {
		while (loop)
		{
			m_clients[id].Send();
			m_clients[id].Recv();
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

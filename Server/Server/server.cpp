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
	int i = 0;
	while(true)
	{
		if (i < MaxClients)
			AcceptNewPlayer(i++);
		else {
			// send game start msg to all clients 
		}
			
		
	}
}

void Server::LoadMap(const char* filename)
{
	ifstream in(filename);
	ObjectInfo object;

	int bead_id = 0, key_id = 0, player_id = 0;
	float mapn;
	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 30; ++j) {
			in >> mapn;
			if (mapn == 0) {					// BEAD
				object.active = true;
				object.x = j;
				object.z = i;
				object.id = bead_id++;
				object.type = ObjectType::BEAD;
				object.boundingOffset = 1.0;	// �ٿ���ڽ� ũ��
				map.beads.push_back(object);
			}
			else if (mapn == 1) {			// KEY
				object.active = true;
				object.x = j;
				object.z = i;
				object.id = key_id++;
				object.type = ObjectType::KEY;
				object.boundingOffset = 1.0;	
				map.keys.push_back(object);
			}
			else if (mapn == 2) {			// WALL
				object.active = true;
				object.x = j;
				object.z = i;
				object.id = 0;
				object.type = ObjectType::WALL;
				object.boundingOffset = 1.0;	
				map.walls.push_back(object);
			}
			else if (mapn == 3) {			// PLAYER_POS
				m_clients[player_id++].GetNewPosition(i, j);
			}
			else if (mapn == 4) {			// DOOR
				map.door.active = true;
				map.door.x = j;
				map.door.z = i;
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
	m_threads.emplace_back(RecvAndSend, id);
	cout << "Accepted new client [" << id << "]\n";
	// TODO: Send join msg to all clients.
	//		 Assign id and role

}

void Server::RecvAndSend(int id)
{
	bool loop = true;

	try {
		while (loop)
		{
			m_clients[id].Recv();
			m_clients[id].ProcessMessage();
			m_clients[id].Send();
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
	
}

void Server::CreateUpdateMapInfoMsg()
{
}

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

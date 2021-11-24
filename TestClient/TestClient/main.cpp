#include "common.h"

int ID;
float x[3], z[3];

void PrintStartGameInfo(start_game& info)
{
	ID = info.my_id;
	x[ID] = info.x[ID];
	z[ID] = info.z[ID];

	for (int i = 0; i < 3; i++)
	{
		x[i] = info.x[i];
		z[i] = info.z[i];
	}

	std::cout << "MyID: " << ID << '\n';
	for (int i = 0; i < 3; i++)
	{
		std::cout<< "pos " << i << " : " << x[i] << " " << z[i] << std::endl;
	}
}

void PrintPlayerInfo(update_player_info& info)
{
	for (int i = 0; i < 3; i++)
	{
		x[info.id[i]] = info.x[info.id[i]];
		z[info.id[i]] = info.z[info.id[i]];
	}

	for (int i = 0; i < 3; i++)
	{
		std::cout << "pos " << i << " : " << x[i] << " " << z[i] << std::endl;
	}
}

void PrintUpdateInfo(update_status& info)
{
	std::cout << "Win: " << (int)info.win << endl;
}

void PrintObjectInfo(object_status& obj)
{
	std::cout << "obj type: " << (int)obj.objType << endl;
	std::cout << "obj pos: " << (int)obj.x << " " << (int)obj.z << endl;
	std::cout << "obj active: " << boolalpha << obj.active << endl;
}

void ProcessPacket(Message& packets)
{
	while (!packets.IsEmpty())
	{
		MsgType msg_type = packets.GetMsgType();
		
		switch (msg_type)
		{
		case MsgType::UPDATE_PLAYER_INFO:
		{
			update_player_info player_info{};
			packets.Pop(reinterpret_cast<char*>(&player_info), sizeof(update_player_info));
			PrintPlayerInfo(player_info);			
			break;
		}
		case MsgType::UPDATE_STATUS:
		{
			int left = packets.PeekNextPacketSize();
			cout << "left: " << std::endl;
			update_status update_info{};
			packets.Pop(reinterpret_cast<char*>(&update_info), sizeof(update_status));
			PrintUpdateInfo(update_info);

			left -= sizeof(update_status);
			while (left > 0)
			{
				object_status obj_info{};
				packets.Pop(reinterpret_cast<char*>(&obj_info), sizeof(object_status));
				PrintObjectInfo(obj_info);
				left -= sizeof(object_status);
			}
			std::cout << left << endl;
			if (!packets.IsEmpty())
				cout << "packet is not empty\n";
		}
		}
	}
}

Message CreateInputMsg()
{
	player_input inputMsg{};
	inputMsg.type = MsgType::PLAYER_INPUT;
	inputMsg.size = sizeof(player_input);
	inputMsg.input = -1;
	inputMsg.x = x[ID];
	inputMsg.z = z[ID];
	Message retMsg{};
	retMsg.Push(reinterpret_cast<char*>(&inputMsg), inputMsg.size);
	return retMsg;
}

void SendAndRecv(Socket& sck)
{
	try {
		// get start_game message
		if (sck.Recv() == 0)
			cout << "What the hell\n";
		start_game game_info{};

		sck.m_recvMsg.Pop(reinterpret_cast<char*>(&game_info), sizeof(start_game));

		PrintStartGameInfo(game_info);

		bool loop = true;
		while (loop)
		{
			Message inputMsg = CreateInputMsg();
			sck.Send(inputMsg);
			std::cout << "Sended input message\n";
			sck.Recv();
			ProcessPacket(sck.m_recvMsg);
		}
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

int main()
{
	try {
		WSAData wData{};
		if (WSAStartup(MAKEWORD(2, 2), &wData) != 0)
			throw Exception("WSAStartup failed");

		Socket sck;
		sck.Init();
		sck.Connect(SERVER_IP, SERVER_PORT);
		thread thrd{ SendAndRecv, std::ref(sck) };

		if (thrd.joinable()) thrd.join();
		WSACleanup();
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}
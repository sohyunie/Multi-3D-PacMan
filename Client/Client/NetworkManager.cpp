#pragma comment(lib, "ws2_32")
#pragma comment (lib, "WS2_32.LIB")
#include "Standard.h"
#include "NetworkManager.h"
#include "InGameManager.h"
//#include <winsock2.h>

NetworkManager* NetworkManager::instance = nullptr;

void NetworkManager::Update()
{
    SendPlayerInput();
    s_socket.Recv();

    Message& packets = s_socket.m_recvMsg;

    while (!packets.IsEmpty())
    {
        MsgType msg_type = packets.GetMsgType();

        switch (msg_type)
        {
        case MsgType::UPDATE_PLAYER_INFO:
        {
            update_player_info player_info{};
            packets.Pop(reinterpret_cast<char*>(&player_info), sizeof(update_player_info));
            // 플레이어의 아이디에 따라 위치 갱신
            for (int i = 0; i < MaxClients; ++i) {
                players[i].x = player_info.x[i];
                players[i].z = player_info.z[i];
            }
            break;
        }
        case MsgType::UPDATE_STATUS:
        {
            int left = packets.PeekNextPacketSize();

            update_status update_info{};
            packets.Pop(reinterpret_cast<char*>(&update_info), sizeof(update_status));
            // Win 상태에 따라 종료화면 처리

            left -= sizeof(update_status);
            while (left > 0)
            {
                object_status obj_info{};
                packets.Pop(reinterpret_cast<char*>(&obj_info), sizeof(object_status));
                // 오브젝트: 비드, 키 그리지 않기(active=false)
                // 문
                left -= sizeof(object_status);
            }
        }
        }
    }
}

bool NetworkManager::GetIsConnected()
{
    return this->isConnected;
}

int NetworkManager::GetMyID()
{
    return this->myID;
}

void NetworkManager::SendPlayerInput()
{
    player_input playerInput;
    playerInput.size = sizeof(player_input);
    playerInput.type = MsgType::PLAYER_INPUT;
    playerInput.x = players[myID].x;
    playerInput.z = players[myID].z;

    inputLock.lock();
    playerInput.input = last_input;
    inputLock.unlock();

    m_sendMsg.Push(reinterpret_cast<char*>(&playerInput), sizeof(playerInput));
    s_socket.Send(m_sendMsg);

    m_sendMsg.Clear();
}

PlayerInfo NetworkManager::GetPlayerInfo(int id)
{
    return this->players[id];
}

void NetworkManager::SetLastInput(char input)
{
    inputLock.lock();
    last_input = input;
    inputLock.unlock();
}

void NetworkManager::Network()
{
    try {
        //cout << "ip 주소를 입력해주세요." << endl;
        //cin.getline(address, sizeof(address));
        //cout << address << "와 연결되었습니다" << endl;

        // 서버 connect
        wcout.imbue(locale("korean"));
        WSADATA WSAData;
        WSAStartup(MAKEWORD(2, 2), &WSAData);

        s_socket = Socket();       
        s_socket.Init();
        s_socket.Connect("127.0.0.1", SERVER_PORT);
        isConnected = true;        

        //int tcp_option = 1;
        //setsockopt(s_socket->m_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&tcp_option), sizeof(tcp_option));

        // 맵 정보 받기.
        s_socket.Recv();
        start_game game_info{};
        s_socket.m_recvMsg.Pop(reinterpret_cast<char*>(&game_info), sizeof(start_game));

        for (int i = 0; i < MaxClients; ++i) {
            players[i].id = game_info.id[i];
            players[i].type = game_info.playertype[i];
            players[i].x = game_info.x[i];
            players[i].z = game_info.z[i];
        }
        InGameManager::GetInstance().GameStart(game_info);

        while (true)
        {
            Update();
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
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
            auto recv_time = std::chrono::system_clock::now();

            duration_time = std::chrono::duration<float>(recv_time - send_time).count();

            update_player_info player_info{};
            packets.Pop(reinterpret_cast<char*>(&player_info), sizeof(update_player_info));
            // 플레이어의 아이디에 따라 위치 갱신
            for (int i = 0; i < MaxClients; ++i) {
                players[i].x = player_info.x[i] - 35;
                players[i].z = player_info.z[i] - 35;
            }
            break;
        }
        case MsgType::UPDATE_STATUS:
        {
            int left = packets.PeekNextPacketSize();

            update_status update_info{};
            packets.Pop(reinterpret_cast<char*>(&update_info), sizeof(update_status));
            // Win 상태에 따라 종료화면 처리
            InGameManager::GetInstance().RecvUpdateStatus(update_info);

            left -= sizeof(update_status);
            while (left > 0)
            {
                object_status obj_info{};
                packets.Pop(reinterpret_cast<char*>(&obj_info), sizeof(object_status));
                // 오브젝트: 비드, 키 그리지 않기(active=false)
                InGameManager::GetInstance().RecvUpdateObject(obj_info);

                // 문
                left -= sizeof(object_status);
            }
            break;
        }
        default:
        {
            std::cout << "Unkown packet\n";
            break;
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
    // Player* players = InGameManager::GetInstance().GetPlayer();
    // int id = InGameManager::GetInstance().GetMyID();

    player_input playerInput;
    playerInput.size = sizeof(player_input);
    playerInput.type = MsgType::PLAYER_INPUT;
    playerInput.x = players[myID].x;
    playerInput.z = players[myID].z;

    playerInput.input = last_input;

    m_sendMsg.Push(reinterpret_cast<char*>(&playerInput), sizeof(playerInput));
    s_socket.Send(m_sendMsg);
    send_time = std::chrono::system_clock::now(); // test
    last_input = -1;

    m_sendMsg.Clear();
}

PlayerInfo NetworkManager::GetPlayerInfo(int id) 
{
    return this->players[id];
}

void NetworkManager::SetLastInput(char input)
{
    last_input = input;
}

void NetworkManager::Network()
{
    try {
        // 서버 connect
        wcout.imbue(locale("korean"));
        WSADATA WSAData;
        WSAStartup(MAKEWORD(2, 2), &WSAData);

        s_socket = Socket();       
        s_socket.Init();
        s_socket.Connect(SERVER_IP, SERVER_PORT);
        isConnected = true;   

        // 맵 정보 받기.
        s_socket.Recv();
        start_game game_info{};
        s_socket.m_recvMsg.Pop(reinterpret_cast<char*>(&game_info), sizeof(start_game));
        this->myID = game_info.my_id;

        for (int i = 0; i < MaxClients; ++i) {
            players[i].id = game_info.id[i];
            players[i].type = game_info.playertype[i];
            players[i].x = game_info.x[i] - 35;
            players[i].z = game_info.z[i] - 35;
        }

        InGameManager::GetInstance().GameStart(game_info);

        while (true)
        {
            Update();
        }
    }
    catch (Exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
#pragma comment(lib, "ws2_32")
#pragma comment (lib, "WS2_32.LIB")
#include "NetworkManager.h"
#include "InGameManager.h"
//#include <winsock2.h>

NetworkManager* NetworkManager::instance = nullptr;

void NetworkManager::Update()
{
    s_socket->Recv();
    BasePacket basePacket = (BasePacket&)*(s_socket->GetRecvMessage().m_buffer);

    MsgType msgType = basePacket.type;
    switch (msgType)
    {
    case MsgType::LOGIN_REQUEST:
        break;
    case MsgType::LOGIN_OK:
        break;
    case MsgType::PLAYER_JOIN:
        break;
    case MsgType::START_GAME:
    {
        start_game startGame = (start_game&)*(s_socket->GetRecvMessage().m_buffer);
        myID = startGame.my_id;
        InGameManager::GetInstance().GameStart(startGame);
        break;
    }
    case MsgType::PLAYER_INPUT:
        break;
    case MsgType::UPDATE_PLAYER_POS:
        break;
    case MsgType::UPDATE_PLAYER_INFO:
        break;
    case MsgType::UPDATE_BEAD:
        break;
    case MsgType::UPDATE_KEY:
        break;
    case MsgType::DOOR_OPEN:
        break;
    }
}

bool NetworkManager::GetIsConnected()
{
    return this->isConnected;
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

        s_socket = new Socket();
        s_socket->Init();
        s_socket->Connect("127.0.0.1", SERVER_PORT);
        isConnected = true;

        

        int tcp_option = 1;
        setsockopt(s_socket->GetSocket(), IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&tcp_option), sizeof(tcp_option));

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
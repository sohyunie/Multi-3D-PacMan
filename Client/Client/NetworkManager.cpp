#pragma comment(lib, "ws2_32")
#pragma comment (lib, "WS2_32.LIB")
#include "NetworkManager.h"
#include "InGameManager.h"
//#include <winsock2.h>

NetworkManager* NetworkManager::instance = nullptr;

// 소켓 함수 오류 출력
void NetworkManager::error_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);

    std::printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

void NetworkManager::Recv()
{
	int len = 0;
	int retval = recvn(s_socket, (char*)&len, sizeof(int), 0);
    if (retval == SOCKET_ERROR)
    {
        throw Exception("Recv Size Error");
    }

	retval = recvn(s_socket, m_recvMessage.m_buffer, len, 0);
    if (retval == SOCKET_ERROR)
    {
        throw Exception("Recv Buffer Error");
    }

}

bool NetworkManager::Send(Message& msg)
{
    int size = sizeof(msg.m_buffer);

    int retval = send(s_socket, (char*)&size, sizeof(size), 0);
    if (retval == SOCKET_ERROR)
    {
        throw Exception("Send Size Error");
    }

    retval = send(s_socket, (char*)&msg.m_buffer, sizeof(msg.m_buffer), 0);
    if (retval == SOCKET_ERROR)
    {
        throw Exception("Send Buffer Error");
    }

    return true;
}

void NetworkManager::Update()
{
    MsgType msgType = msgtype;
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
        //RecvStartGame startGame = (RecvStartGame&)*(m_recvMessage.m_buffer + sizeof(double));
        myID =  m_recvMessage.m_buffer[6];
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

// 사용자 정의 데이터 수신 함수
int NetworkManager::recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
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
        s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
        SOCKADDR_IN server_addr;
        ZeroMemory(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, address, &server_addr.sin_addr);
        int ret = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));		// reinterret_cast : type casting
        cout << "ret : " << ret << endl;
        if (SOCKET_ERROR == ret) {
            error_display("Connect error");
        }
        else
        {
            isConnected = true;
        }

        int tcp_option = 1;
        setsockopt(s_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&tcp_option), sizeof(tcp_option));

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

Message::Message()
    : m_writeIndex(0),
    m_readIndex(0),
    m_remainSize(MaxBufferSize)
{
    std::memset(m_buffer, 0, MaxBufferSize);
}

Message::~Message()
{
}
#pragma once
#include "Standard.h"

#define SERVERIP   "127.0.0.1"
#define BUFSIZE 2048
#define BUFSIZE2 128

class NetworkManager
{
private:
    static NetworkManager* instance;
    char buf[BUFSIZE2 + 1]; // 데이터 송수신 버퍼

    Socket* s_socket;

    char g_recv_buf[BUFSIZE];
    WSABUF mybuf_r;
    WSABUF mybuf;
    bool isConnected = false;
    Message m_recvMessage;		// Recv에 사용되는 메시지 버퍼
    Message m_sendMessage;		// Send에 사용되는 메시지 버퍼

    MsgType msgtype;

    char myID;
public:
    static NetworkManager& GetInstance() {
        if (instance == NULL) {
            instance = new NetworkManager();
        }
        return *instance;
    }
    // 대화상자 프로시저
    void Network();
    void Update();
    bool GetIsConnected();
};
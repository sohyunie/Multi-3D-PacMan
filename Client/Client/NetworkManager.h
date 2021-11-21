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
    Message m_sendMsg;
    WSABUF mybuf_r;
    WSABUF mybuf;
    bool isConnected = false;

    MsgType msgtype;

    char myID;

public:
    static NetworkManager& GetInstance() {
        if (instance == NULL) {
            instance = new NetworkManager();
        }
        return *instance;
    }

    void Network();
    void Update();
    bool GetIsConnected();
    void SendPlayerInput(float x, float z, char input);
};
#pragma once
#include "Standard.h"

#define SERVERIP   "127.0.0.1"
#define BUFSIZE 2048
#define BUFSIZE2 128

class NetworkManager
{
private:
    static NetworkManager* instance;
    char buf[BUFSIZE2 + 1]; // ������ �ۼ��� ����

    Socket* s_socket;

    char g_recv_buf[BUFSIZE];
    WSABUF mybuf_r;
    WSABUF mybuf;
    bool isConnected = false;
    Message m_recvMessage;		// Recv�� ���Ǵ� �޽��� ����
    Message m_sendMessage;		// Send�� ���Ǵ� �޽��� ����

    MsgType msgtype;

    char myID;
public:
    static NetworkManager& GetInstance() {
        if (instance == NULL) {
            instance = new NetworkManager();
        }
        return *instance;
    }
    // ��ȭ���� ���ν���
    void Network();
    void Update();
    bool GetIsConnected();
};
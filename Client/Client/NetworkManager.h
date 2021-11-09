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

    SOCKET s_socket;

    char g_recv_buf[BUFSIZE];
    WSABUF mybuf_r;
    WSABUF mybuf;
    bool isConnected = false;
    Message m_recvMessage;		// Recv�� ���Ǵ� �޽��� ����
    Message m_sendMessage;		// Send�� ���Ǵ� �޽��� ����

    int myID;
public:
    static NetworkManager& GetInstance() {
        if (instance == NULL) {
            instance = new NetworkManager();
        }
        return *instance;
    }
    // ��ȭ���� ���ν���
    int recvn(SOCKET s, char* buf, int len, int flags);
    MsgType Recv();
    bool Send(Message& msg);
    void Network();
    void error_display(const char* msg);
    void do_recv();
    void do_send(int input);
    void Update();
    bool GetIsConnected();
};
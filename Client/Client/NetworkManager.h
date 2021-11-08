#pragma once
#include "Standard.h"

#define SERVERIP   "127.0.0.1"
#define BUFSIZE 2048
#define BUFSIZE2 128

class Message
{
public:
    Message();
    ~Message();

    static const int MaxBufferSize = 1024;

    char* MsgBuffer = m_buffer;	// 임시로 만들어논 것
private:
    char m_buffer[MaxBufferSize];
    int m_readIndex;
    int m_writeIndex;
    int m_remainSize;
};

class NetworkManager
{
private:
    static NetworkManager* instance;
    char buf[BUFSIZE2 + 1]; // 데이터 송수신 버퍼

    char address[128] = "127.0.0.1";
    SOCKET s_socket;
    const short SERVER_PORT = 7777;

    char g_recv_buf[BUFSIZE];
    WSABUF mybuf_r;
    WSABUF mybuf;
    bool isConnected = false;
    Message m_recvMessage;		// Recv에 사용되는 메시지 버퍼
    Message m_sendMessage;		// Send에 사용되는 메시지 버퍼

    int myID;
public:
    static NetworkManager& GetInstance() {
        if (instance == NULL) {
            instance = new NetworkManager();
        }
        return *instance;
    }
    // 대화상자 프로시저
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
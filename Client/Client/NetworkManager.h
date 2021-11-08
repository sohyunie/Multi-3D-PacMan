#pragma once
#include "Standard.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 2048
#define BUFSIZE2 128

enum class MsgType : char
{
    LOGIN_REQUEST,
    LOGIN_OK,
    PLAYER_JOIN,
    START_GAME,
    PLAYTER_INPUT,
    UPDATE_PLAYER_POS,
    UPDATE_PLAYER_INFO,
    UPDATE_BEAD,
    UPDATE_KEY,
    DOOR_OPEN,
    NONE,
};

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

enum class WinStatus : char
{
    NONE,
    RUNNER_WIN,
    TAGGER_WIN
};

enum class PlayerType : char
{
    TAGGER,
    RUNNER
};

struct Base
{
    short size;
    MsgType type;
};

// Send Struct
struct SendPlayerInput : Base
{
    char input;
    float x;
    float z;
};

// Recv Struct
struct RecvPlayerJoin : Base
{
    char totalPlayers;
};

struct RecvStartGame : Base
{
    char id[3];
    PlayerType playertype[3];
    float x[3];
    float z[3];
    char mapinfo[30][30];
};

struct RecvUpdatePlayerInfo : Base
{
    char id[3];
    float x[3];
    float z[3];
};

struct RecvUpdateStatus : Base
{
    WinStatus win;
    char hp;
    ObjectType objectType;
    char id;
    bool active;
};

#pragma once
#include "Standard.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 2048
#define BUFSIZE2 128

class NetworkManager
{
private:
    static NetworkManager* instance;
    char buf[BUFSIZE2 + 1]; // 데이터 송수신 버퍼

    char address[128];
    SOCKET s_socket;
    const short SERVER_PORT = 4000;

    char g_recv_buf[BUFSIZE];
    WSABUF mybuf_r;
    WSABUF mybuf;
    bool isConnected = false;
public:
    static NetworkManager& GetInstance() {
        if (instance == NULL) {
            instance = new NetworkManager();
        }
        return *instance;
    }
    // 대화상자 프로시저
    void CALLBACK recv_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
    void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
    int recvn(SOCKET s, char* buf, int len, int flags);
    void Network();
    void error_display(int err_no);
    void do_recv();
    void do_send(int input);
    bool GetIsConnected();
};

enum class MsgType
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
    DOOR_OPEN
};

enum class WinStatus : char
{
    NONE,
    RUNNER_WIN,
    TAGGER_WIN
};

enum class PlayerType
{
    RUNNER,
    TAGGER
};

// Send Struct
struct SendPlayerInput
{
    short size;
    MsgType type;
    char input;
    float x;
    float z;
};

// Recv Struct
struct RecvPlayerJoin
{
    short size;
    MsgType type;
    char totalPlayers;
};

struct RecvStartGame
{
    short size;
    MsgType type;
    char id[3];
    PlayerType playertype[3];
    float x[3];
    float z[3];
    char mapinfo[30][30];
};

struct RecvUpdatePlayerInfo
{
    short size;
    MsgType type;
    char id[3];
    float x[3];
    float z[3];
};

struct RecvUpdateStatus
{
    short size;
    MsgType type;
    WinStatus win;
    char hp;
    ObjectType objectType;
    char id;
    bool active;
};

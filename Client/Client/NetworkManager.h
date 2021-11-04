#pragma once
#include "Standard.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 2048
#define BUFSIZE2 128

extern SOCKET sock; // 소켓
extern char buf[BUFSIZE2 + 1]; // 데이터 송수신 버퍼
extern HANDLE hWrite_Event, hRead_Event, Update_Event;


// 대화상자 프로시저
void CALLBACK recv_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
int recvn(SOCKET s, char* buf, int len, int flags);

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

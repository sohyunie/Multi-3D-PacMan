#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <mutex>
#include <condition_variable>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const short SERVER_PORT = 7777;			
const char* const SERVER_IP = "127.0.0.1";

static const int MaxBufferSize = 1024;

enum class MsgType : char		// 메시지를 식별할 수 있는 메시지 형식
{	
	LOGIN_REQUEST,				// 로그인 요청
	LOGIN_OK,							// 로그인 확인
	PLAYER_JOIN,					// 플레이어 입장
	START_GAME,					// 게임 시작
	PLAYER_INPUT,					// 플레이어 키 입력
	UPDATE_PLAYER_INFO,		// 플레이어 정보
	UPDATE_STATUS
};

enum class PlayerType : char	// 플레이어 타입
{
	TAGGER,							// 술래
	RUNNER							// 도망자
};

enum class ObjectType : char	// 오브젝트 타입
{
	BEAD,								// 비드
	KEY,									// 키
	DOOR,								// 탈출구
	WALL,								// 벽
	NONE,								// 아무것도 없음

	// Client Only
	PLAYER,
	GHOST,
	TEXTURE,
	POWERBEAD,
	BOTTOM,
	ROAD,
};

enum class WinStatus : char		// 승리 상태
{
	NONE,									// 게임 진행
	RUNNER_WIN,						// 러너 승리
	TAGGER_WIN							// 태거 승리
};

enum class Direction : char
{
	UP,
	DOWN,
	LEFT,
	RIGHT,

	// Client Only
	NONE,
};

struct Vector4				// 객체의 바운딩 박스 표현하기 위해 사용하는 vector4 구조체 
{
	float MaxX;				
	float MinX;				
	float MaxZ;
	float MinZ;
};

#pragma pack(push, 1)
struct player_join			// 플레이어가 추가되면 총 플레이어의 수를 보내어 대기 화면을 업데이트한다. 
{
	short size;
	MsgType type;
	char TotalPlayers;
};

struct start_game	// 게임을 시작하면, 클라이언트는 모든 플레이어의 위치정보와 타입, id를 저장한다.
{
	short size;
	MsgType type;
	char id[3];
	char my_id;
	PlayerType playertype[3];
	float x[3];
	float z[3];
	char mapinfo[30][30];
};

struct player_input		//클라이언트는 지속적으로 서버에게 입력 값을 보낸다. (입력할 때 마다)
{
	short size;
	MsgType type;
	char input;	// Left : 0 Right : 1
	float x;
	float z;
};

struct update_player_info			// 서버는 지속적으로 클라이언트들의 위치를 갱신하여 보낸다. (1/30초마다)
{
	short size;
	MsgType type;
	char id[3];
	float x[3];
	float z[3];
};

struct object_status
{
	ObjectType objType;
	char id;
	bool active;
};

struct update_status			//맵에 존재하는 오브젝트들의 변화되어 사라졌는지, 그 상태를 보낸다.(충돌할 때 마다)
{
	short size;
	MsgType type;
	WinStatus win;
	char hp;
	/*ObjectType objType;
	char id;
	bool active;*/
};
#pragma pack(pop)

struct ObjectInfo			// 오브젝트의 정보를 관리하는 구조체
{
	char id;										// 오브젝트 아이디
	ObjectType type;						// 오브젝트 타입
	float x;									// 오브젝트 x 좌표
	float z;									// 오브젝트 z 좌표
	bool active;							// 오브젝트 활성화 여부
	float boundingOffset;				// 오브젝트 바운드 오프셋

	Vector4 GetBoundingBox()		// 오브젝트 바운딩박스 상자 정보 반환
	{
		Vector4 box =
		{
			x + boundingOffset,
			x - boundingOffset,
			z + boundingOffset,
			z - boundingOffset
		};
		return box;
	}

};

struct MapInfo							// 맵에 존재하는 모든 오브젝트를 담고 있는 구조체
{
	vector<ObjectInfo> beads;		// 비드들을 담는 벡터
	vector<ObjectInfo> keys;			// 키를 담고 있는 배열
	vector<ObjectInfo> walls;			// 벽 오브젝트를 담는 배열
	ObjectInfo door;						// 탈출구 오브젝트
};
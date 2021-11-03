#pragma once
#include<iostream>
#include<thread>
#include<mutex>
#include<vector>
#include <array>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const short SERVER_PORT = 7777;			// ���� ��Ʈ ��ȣ
const char* const SERVER_IP = "127.0.0.1";		// ���� IP �ּ�

enum class MsgType : char		// �޽����� �ĺ��� �� �ִ� �޽��� ����
{	
	LOGIN_REQUEST,				// �α��� ��û
	LOGIN_OK,							// �α��� Ȯ��
	PLAYER_JOIN,					// �÷��̾� ����
	START_GAME,					// ���� ����
	PLAYER_INPUT,					// �÷��̾� Ű �Է�
	UPDATE_PLAYER_POS,		// �÷��̾� ��ġ
	UPDATE_PLAYER_INFO,		// �÷��̾� ����
	UPDATE_BEAD,					// ���
	UPDATE_KEY,						// Ű
	DOOR_OPEN						// ���� Ż�ⱸ
};

enum class PlayerType : char	// �÷��̾� Ÿ��
{
	TAGGER,							// ����
	RUNNER							// ������
};

enum class ObjectType : char	// ������Ʈ Ÿ��
{
	BEAD,								// ���
	KEY,									// Ű
	DOOR,								// Ż�ⱸ
	WALL									// ��
};

enum class InputType : char		// �÷��̾� ���� �Է��� ��Ÿ���� Ÿ��
{
	UP,									// �츮��
	DOWN,								// �¿�ۿ����µ�?
	LEFT,								// ���� ����Ű
	RIGHT								// ���� ����Ű
};

enum class WinStatus : char		// �¸� ����
{
	NONE,									// ���� ����
	RUNNER_WIN,						// ���� �¸�
	TAGGER_WIN							// �°� �¸�
};

enum class Direction : char
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct Vector4				// ��ü�� �ٿ�� �ڽ� ǥ���ϱ� ���� ����ϴ� vector4 ����ü 
{
	float MaxX;				
	float MinX;				
	float MaxZ;
	float MinZ;
};

struct player_join			// �÷��̾ �߰��Ǹ� �� �÷��̾��� ���� ������ ��� ȭ���� ������Ʈ�Ѵ�. 
{
	short size;
	MsgType type;
	char TotalPlayers;
};

struct start_game	// ������ �����ϸ�, Ŭ���̾�Ʈ�� ��� �÷��̾��� ��ġ������ Ÿ��, id�� �����Ѵ�.
{
	short size;
	MsgType type;
	char id[3];
	PlayerType playertype[3];
	float x[3];
	float z[3];
	char mapinfo[30][30];
};

struct player_input		//Ŭ���̾�Ʈ�� ���������� �������� �Է� ���� ������. (�Է��� �� ����)
{
	short size;
	MsgType type;
	char input;
	float x;
	float z;
};

struct update_player_info			// ������ ���������� Ŭ���̾�Ʈ���� ��ġ�� �����Ͽ� ������. (1/30�ʸ���)
{
	short size;
	MsgType type;
	char id[3];
	float x[3];
	float z[3];
};

struct update_status			//�ʿ� �����ϴ� ������Ʈ���� ��ȭ�Ǿ� ���������, �� ���¸� ������.(�浹�� �� ����)
{
	short size;
	MsgType type;
	WinStatus win;
	char hp;
	ObjectType objType;
	char id;
	bool active;
};

struct ObjectInfo			// ������Ʈ�� ������ �����ϴ� ����ü
{
	int id;										// ������Ʈ ���̵�
	ObjectType type;						// ������Ʈ Ÿ��
	float x;									// ������Ʈ x ��ǥ
	float z;									// ������Ʈ z ��ǥ
	bool active;							// ������Ʈ Ȱ��ȭ ����
	float boundingOffset;				// ������Ʈ �ٿ�� ������

	Vector4 GetBoundingBox();		// ������Ʈ �ٿ���ڽ� ���� ���� ��ȯ
};

struct MapInfo							// �ʿ� �����ϴ� ��� ������Ʈ�� ��� �ִ� ����ü
{
	vector<ObjectInfo> beads;		// ������ ��� ����
	vector<ObjectInfo> keys;			// Ű�� ��� �ִ� �迭
	vector<ObjectInfo> walls;			// �� ������Ʈ�� ��� �迭
	ObjectInfo door;						// Ż�ⱸ ������Ʈ
};
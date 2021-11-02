#pragma once
#include<iostream>
#include<WS2tcpip.h>
#include<thread>
#include<mutex>
#include<vector>
using namespace std;

const short SERVER_PORT = 7777;			// ���� ��Ʈ ��ȣ
const char* SERVER_IP = "127.0.0.1";		// ���� IP �ּ�

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

struct Vector4				// ��ü�� �ٿ�� �ڽ� ǥ���ϱ� ���� ����ϴ� vector4 ����ü 
{
	float MaxX;				
	float MinX;				
	float MaxZ;
	float MinZ;
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

class Message {								// ���� �޽����� �ϳ��� ���۷� �����ϰ� ���� �޽����� �ٽ� �и��ϴ� �Լ�
private:
	static const int MaxBufferSize = 1024;			// �ִ� ���� ������
	char MsgBuffer[MaxBufferSize];					// �޽��� ����
	int BeginOffset;											// ���ۿ��� �޽����� �����ϴ� ���� �ε���
	int EndOffset;												// ���ۿ��� �޽����� ������ ���� �ε���
public:
	void CopyData(void* msg, int size);				// �޽����� size��ŭ MsgBuffer�� �����ϴ� �Լ�, EndOffset�� ������
	void ReleaseData(void* msg);						// ���ۿ� �ִ� �����͸� �ϳ��� �޽����� �����ϴ� �Լ�, BeginOffset�� ������
};

class Socket {
private:
	SOCKET socket;					// ���� �ڵ�
	Message recvMessage;		// Recv�� ���Ǵ� �޽��� ����
public:
	void Bind(short ServerPort);														// ��Ʈ ��ȣ�� ���� ���ε�
	void Listen();																			// �����ϴ� ȣ��Ʈ �ޱ�
	void Connect(const char* ServerAddress, short ServerPort);		// ���� �ּҷ� ���� �õ�
	SOCKET Accept();																	// Ŭ���̾�Ʈ ������ ���� ��ȯ
	void Send(Message& msg);														// �޽����� ��� ȣ��Ʈ���� �����Ѵ�
	void Recv();																			// ��� ȣ��Ʈ�� ���� �޽����� recvMessage�� �����Ѵ�.
};

class Exception : exception {		// ��Ʈ��ũ �Լ����� ������ �߻��� �� ������ ������ִ� Ŭ����
private:
	const char* ErrorString;			// ���� ������ ��� ���ڿ�
public:
	Exception(int error_code);		// �����ڵ带 �޾� ���� ������ ��ȯ�ϴ� ������
	virtual const char* what();		// exception Ŭ���� �������̵� �Լ��ν� �����޽����� ��ȯ
};
#pragma once
#include<iostream>
#include<WS2tcpip.h>
#include<thread>
#include<mutex>
#include<vector>
using namespace std;

const short SERVER_PORT = 7777;			// 서버 포트 번호
const char* SERVER_IP = "127.0.0.1";		// 서버 IP 주소

enum class MsgType : char		// 메시지를 식별할 수 있는 메시지 형식
{	
	LOGIN_REQUEST,				// 로그인 요청
	LOGIN_OK,							// 로그인 확인
	PLAYER_JOIN,					// 플레이어 입장
	START_GAME,					// 게임 시작
	PLAYER_INPUT,					// 플레이어 키 입력
	UPDATE_PLAYER_POS,		// 플레이어 위치
	UPDATE_PLAYER_INFO,		// 플레이어 정보
	UPDATE_BEAD,					// 비드
	UPDATE_KEY,						// 키
	DOOR_OPEN						// 최종 탈출구
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
	WALL									// 벽
};

enum class InputType : char		// 플레이어 현재 입력을 나타내는 타입
{
	UP,									// 우리는
	DOWN,								// 좌우밖에없는데?
	LEFT,								// 좌측 방향키
	RIGHT								// 우측 방향키
};

struct Vector4				// 객체의 바운딩 박스 표현하기 위해 사용하는 vector4 구조체 
{
	float MaxX;				
	float MinX;				
	float MaxZ;
	float MinZ;
};

struct ObjectInfo			// 오브젝트의 정보를 관리하는 구조체
{
	int id;										// 오브젝트 아이디
	ObjectType type;						// 오브젝트 타입
	float x;									// 오브젝트 x 좌표
	float z;									// 오브젝트 z 좌표
	bool active;							// 오브젝트 활성화 여부
	float boundingOffset;				// 오브젝트 바운드 오프셋

	Vector4 GetBoundingBox();		// 오브젝트 바운딩박스 상자 정보 반환
};

struct MapInfo							// 맵에 존재하는 모든 오브젝트를 담고 있는 구조체
{
	vector<ObjectInfo> beads;		// 비드들을 담는 벡터
	vector<ObjectInfo> keys;			// 키를 담고 있는 배열
	vector<ObjectInfo> walls;			// 벽 오브젝트를 담는 배열
	ObjectInfo door;						// 탈출구 오브젝트
};

class Message {								// 여러 메시지를 하나의 버퍼로 구성하고 여러 메시지로 다시 분리하는 함수
private:
	static const int MaxBufferSize = 1024;			// 최대 버퍼 사이즈
	char MsgBuffer[MaxBufferSize];					// 메시지 버퍼
	int BeginOffset;											// 버퍼에서 메시지가 시작하는 곳의 인덱스
	int EndOffset;												// 버퍼에서 메시지가 끝나는 곳의 인덱스
public:
	void CopyData(void* msg, int size);				// 메시지를 size만큼 MsgBuffer로 복사하는 함수, EndOffset이 증가함
	void ReleaseData(void* msg);						// 버퍼에 있는 데이터를 하나의 메시지에 복사하는 함수, BeginOffset이 증가함
};

class Socket {
private:
	SOCKET socket;					// 소켓 핸들
	Message recvMessage;		// Recv에 사용되는 메시지 버퍼
public:
	void Bind(short ServerPort);														// 포트 번호로 소켓 바인딩
	void Listen();																			// 접근하는 호스트 받기
	void Connect(const char* ServerAddress, short ServerPort);		// 서버 주소로 연결 시도
	SOCKET Accept();																	// 클라이언트 연결후 소켓 반환
	void Send(Message& msg);														// 메시지를 상대 호스트에게 전송한다
	void Recv();																			// 상대 호스트가 보낸 메시지를 recvMessage에 저장한다.
};

class Exception : exception {		// 네트워크 함수에서 에러가 발생할 시 오류를 출력해주는 클래스
private:
	const char* ErrorString;			// 오류 정보를 담는 문자열
public:
	Exception(int error_code);		// 에러코드를 받아 에러 정보를 반환하는 생성자
	virtual const char* what();		// exception 클래스 오버라이드 함수로써 오류메시지를 반환
};
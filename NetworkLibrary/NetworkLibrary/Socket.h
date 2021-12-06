#pragma once

#include "Protocol.h"
#include "Message.h"

class Socket 
{
public:
	SOCKET m_socket;					// 소켓 핸들
	char m_recvBuffer[MaxBufferSize];		// Recv에 사용되는 메시지 버퍼
	
public:
	Message m_recvMsg;

public:
	Socket();
	virtual ~Socket();

	void Init();
	void Bind(short ServerPort);// 포트 번호로 소켓 바인딩
	void Listen();																			// 접근하는 호스트 받기
	void Connect(const char* ServerAddress, short ServerPort);		// 서버 주소로 연결 시도
	SOCKET Accept();																	// 클라이언트 연결후 소켓 반환
	int Send(Message& msg);														// 메시지를 상대 호스트에게 전송한다
	int Recv();
};
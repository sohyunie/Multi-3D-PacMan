#pragma once

#include "Protocol.h"
#include "Message.h"

class Socket 
{
protected:
	SOCKET m_socket;					// ���� �ڵ�
	char m_recvBuffer[MaxBufferSize];		// Recv�� ���Ǵ� �޽��� ����
	
public:
	Message m_recvMsg;

public:
	Socket();
	virtual ~Socket();

	void Init();
	void Bind(short ServerPort);														// ��Ʈ ��ȣ�� ���� ���ε�
	void Listen();																			// �����ϴ� ȣ��Ʈ �ޱ�
	void Connect(const char* ServerAddress, short ServerPort);		// ���� �ּҷ� ���� �õ�
	SOCKET Accept();																	// Ŭ���̾�Ʈ ������ ���� ��ȯ
	void Send(Message& msg);														// �޽����� ��� ȣ��Ʈ���� �����Ѵ�
	void Recv();
};
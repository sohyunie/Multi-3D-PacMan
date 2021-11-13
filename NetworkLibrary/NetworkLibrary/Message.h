#pragma once

class Message {	// 여러 메시지를 하나의 버퍼로 구성하고 여러 메시지로 다시 분리하는 함수
public:
	static const int MaxBufferSize = 1024;			// 최대 버퍼 사이즈
	char MsgBuffer[MaxBufferSize];					// 메시지 버퍼
	int BeginOffset;											// 버퍼에서 메시지가 시작하는 곳의 인덱스
	int EndOffset;												// 버퍼에서 메시지가 끝나는 곳의 인덱스

public:
	Message();
	~Message();

	//void Push(char* msg, int size);
	//void Pop(char* msg, int size);

	//bool IsEmpty();
	//bool IsFull();

	short PeekSize();

	char m_buffer[MaxBufferSize];
private:
	int m_readIndex;
	int m_writeIndex;
	int m_remainSize;
	void CopyData(void* msg, int size);			// 메시지를 size만큼 MsgBuffer로 복사하는 함수, EndOffset이 증가함
	void ReleaseData(void* msg);					// 버퍼에 있는 데이터를 하나의 메시지에 복사하는 함수, BeginOffset이 증가
};
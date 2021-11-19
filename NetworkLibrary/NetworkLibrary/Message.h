#pragma once

#include "Protocol.h"

class Message
{
public:
	Message();
	~Message();

	void Clear();

	void Push(char* msg, int size);
	void Pop(char* msg, int size);

	bool IsEmpty();
	bool IsFull();

	MsgType GetMsgType();
	short GetTotalMsgSize();
	short PeekNextPacketSize();

	char m_buffer[MaxBufferSize];
private:
	int m_readIndex;
	int m_writeIndex;
	int m_remainSize;
};
#pragma once

class Message
{
public:
	Message();
	~Message();

	void Push(char* msg, int size);
	void Pop(char* msg, int size);

	bool IsEmpty();
	bool IsFull();

	static const int MaxBufferSize = 1024;

	char* MsgBuffer = m_buffer;	// 임시로 만들어논 것
private:
	char m_buffer[MaxBufferSize];
	int m_readIndex;
	int m_writeIndex;
	int m_remainSize;
};
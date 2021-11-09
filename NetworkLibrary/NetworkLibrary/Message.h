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

	char m_buffer[MaxBufferSize];
private:
	int m_readIndex;
	int m_writeIndex;
	int m_remainSize;
};
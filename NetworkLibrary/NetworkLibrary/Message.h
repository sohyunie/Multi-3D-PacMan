#pragma once

class Message {	// ���� �޽����� �ϳ��� ���۷� �����ϰ� ���� �޽����� �ٽ� �и��ϴ� �Լ�
public:
	static const int MaxBufferSize = 1024;			// �ִ� ���� ������
	char MsgBuffer[MaxBufferSize];					// �޽��� ����
	int BeginOffset;											// ���ۿ��� �޽����� �����ϴ� ���� �ε���
	int EndOffset;												// ���ۿ��� �޽����� ������ ���� �ε���

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
	void CopyData(void* msg, int size);			// �޽����� size��ŭ MsgBuffer�� �����ϴ� �Լ�, EndOffset�� ������
	void ReleaseData(void* msg);					// ���ۿ� �ִ� �����͸� �ϳ��� �޽����� �����ϴ� �Լ�, BeginOffset�� ����
};
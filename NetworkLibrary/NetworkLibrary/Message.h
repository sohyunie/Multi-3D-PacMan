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

	void CopyData(void* msg, int size);			// �޽����� size��ŭ MsgBuffer�� �����ϴ� �Լ�, EndOffset�� ������
	void ReleaseData(void* msg);					// ���ۿ� �ִ� �����͸� �ϳ��� �޽����� �����ϴ� �Լ�, BeginOffset�� ����
};
#pragma once

#include <exception>

class Exception : std::exception {		// ��Ʈ��ũ �Լ����� ������ �߻��� �� ������ ������ִ� Ŭ����
private:
	const char* ErrorString;			// ���� ������ ��� ���ڿ�
public:
	Exception(int error_code);		// �����ڵ带 �޾� ���� ������ ��ȯ�ϴ� ������
	virtual const char* what();		// exception Ŭ���� �������̵� �Լ��ν� �����޽����� ��ȯ
};
#pragma once

#include <exception>
#include <string>

class Exception : std::exception {		// ��Ʈ��ũ �Լ����� ������ �߻��� �� ������ ������ִ� Ŭ����
private:
	std::string ErrorString;			// ���� ������ ��� ���ڿ�
public:
	Exception(const char* error_msg);		// �����ڵ带 �޾� ���� ������ ��ȯ�ϴ� ������
	virtual const char* what();		// exception Ŭ���� �������̵� �Լ��ν� �����޽����� ��ȯ
};
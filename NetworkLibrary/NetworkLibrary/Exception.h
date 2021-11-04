#pragma once

#include <exception>
#include <string>

class Exception : std::exception {		// 네트워크 함수에서 에러가 발생할 시 오류를 출력해주는 클래스
private:
	std::string ErrorString;			// 오류 정보를 담는 문자열
public:
	Exception(const char* error_msg);		// 에러코드를 받아 에러 정보를 반환하는 생성자
	virtual const char* what();		// exception 클래스 오버라이드 함수로써 오류메시지를 반환
};
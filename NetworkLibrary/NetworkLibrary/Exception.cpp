#include "Exception.h"

Exception::Exception(int error_code)
{
	// errorStr ��ȯ
}

const char* Exception::what()
{
	return ErrorString;
}

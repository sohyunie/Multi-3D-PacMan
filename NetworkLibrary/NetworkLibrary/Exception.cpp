#include "Exception.h"

Exception::Exception(int error_code)
{
	// errorStr º¯È¯
}

const char* Exception::what()
{
	return ErrorString;
}

#include "Exception.h"
#include "Protocol.h"

Exception::Exception(const char* error_msg)
{
	LPSTR msgBuffer;
	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&msgBuffer, 0, NULL);

	ErrorString = error_msg;
	ErrorString += ": ";
	ErrorString += msgBuffer;
}

const char* Exception::what()
{
	return ErrorString.c_str();
}

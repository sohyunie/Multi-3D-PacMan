#include "Message.h"

Message::Message()
	: MsgBuffer{},
	  BeginOffset(0),
	  EndOffset(0)
{
}

Message::~Message()
{
}

void Message::CopyData(void* msg, int size)
{
}

void Message::ReleaseData(void* msg)
{
}

short Message::PeekSize()
{
	short size = 0;
	std::memcpy(reinterpret_cast<char*>(&size), m_buffer, sizeof(short));
	return size;
}

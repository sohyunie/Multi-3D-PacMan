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

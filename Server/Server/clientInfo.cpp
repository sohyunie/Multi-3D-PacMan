#include "clientInfo.h"

ClientInfo::ClientInfo()
{
}

ClientInfo::~ClientInfo()
{
}

Vector4 ClientInfo::GetBoundingBox()
{
	return Vector4();
}

void ClientInfo::ProcessPacket(char* packets)
{
}

void ClientInfo::CheckObjectsStatus()
{
}

void ClientInfo::IsCollided(Vector4& a, Vector4& b)
{
}

pair<float, float> ClientInfo::GetNewPosition()
{
	return pair<float, float>();
}

void ClientInfo::CreateLoginOkAndMapInfoMsg()
{
}

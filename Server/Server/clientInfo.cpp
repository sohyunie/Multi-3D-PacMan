#include "clientInfo.h"

ClientInfo::ClientInfo()
	: m_id(0), 
	  m_type(PlayerType::RUNNER),
	  m_hp(0),
	  m_pos_x(0.0f),
	  m_pos_z(0.0f),
	  m_boundingOffset(0.0f),
	  m_sendMsg({}),
	  m_direction(Direction::UP)
{
}

ClientInfo::ClientInfo(SOCKET sck, int id)
	: ClientInfo()
{
	m_id = id;
	m_socket = sck;
}

ClientInfo::~ClientInfo()
{
}

Vector4 ClientInfo::GetBoundingBox()
{
	return Vector4();
}

void ClientInfo::Send()
{
}

void ClientInfo::ProcessMessage()
{
	std::string msg(m_recvMessage.MsgBuffer);
	cout << msg << endl;
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

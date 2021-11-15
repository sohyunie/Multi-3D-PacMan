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
	Vector4 box =
	{
		m_pos_x + m_boundingOffset,
		m_pos_x - m_boundingOffset,
		m_pos_z + m_boundingOffset,
		m_pos_z - m_boundingOffset
	};
	return box;
}

void ClientInfo::Send()
{
	Socket::Send(m_sendMsg);
}

void ClientInfo::ProcessMessage()
{
	std::string msg(m_recvMessage.m_buffer);
	//cout << msg << endl;
}

void ClientInfo::CheckObjectsStatus(MapInfo& map)
{
	const Vector4& clientBB = GetBoundingBox();
	for (ObjectInfo& bead : map.beads)
	{
		const Vector4& beadBB = bead.GetBoundingBox();
		if (IsCollided(clientBB, beadBB))
		{
			bead.active = false;
		}
	}
	for (ObjectInfo& key : map.keys)
	{
		const Vector4& keyBB = key.GetBoundingBox();
		if (IsCollided(clientBB, keyBB))
		{
			key.active
		}
	}
	const Vector4& doorBB = map.door.GetBoundingBox();
	if (IsCollided(clientBB, doorBB))
	{
		
	}
}

bool ClientInfo::IsCollided(const Vector4& a, const Vector4& b)
{
	if (a.MinX > b.MaxX || b.MinX > a.MinX)
		return false;

	if (a.MinZ > b.MaxZ || b.MinZ > a.MaxZ)
		return false;

	return true;
}

pair<float, float> ClientInfo::GetNewPosition()
{
	return pair<float, float>();
}

void ClientInfo::CreateLoginOkAndMapInfoMsg(start_game& s_game)
{
	m_sendMsg.Push(reinterpret_cast<char*>(&s_game), sizeof(s_game));
}

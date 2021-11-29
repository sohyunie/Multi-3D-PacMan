#include "clientInfo.h"

ClientInfo::ClientInfo()
	:	m_id(0),
		m_type(PlayerType::RUNNER),
		m_hp(0),
		m_pos_x(0.0f),
		m_pos_z(0.0f),
		m_boundingOffset(0.0f),
		m_sendMsg({}),
		m_direction(Direction::UP),
		m_active(false)
{
}

ClientInfo::~ClientInfo()
{
}

void ClientInfo::Init(SOCKET sck, int id)
{
	m_id = id;
	m_socket = sck;
	m_active = true;
}

void ClientInfo::SendMsg()
{
	Send(m_sendMsg);
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

void ClientInfo::ProcessMessage()
{
	while (!m_recvMsg.IsEmpty())
	{
		MsgType msg_type = m_recvMsg.GetMsgType();

		switch (msg_type)
		{
		case MsgType::PLAYER_INPUT:
		{
			std::cout << "Received input message\n";
			player_input pi{};
			m_recvMsg.Pop(reinterpret_cast<char*>(&pi), sizeof(player_input));
			ChangeDirection(pi);			
			break;
		}
		}
	}
}

bool ClientInfo::Collied(const Vector4& a, const Vector4& b)
{
	if (a.MinX > b.MaxX || b.MinX > a.MaxX)
		return false;
	if(a.MinZ > b.MaxZ || b.MinZ > a.MaxZ)
		return false;

	return true;
}

bool ClientInfo::MapCollied(MapInfo& map)
{
	const Vector4& clientBB = GetBoundingBox();

	for (ObjectInfo& wall : map.walls)
	{
		const Vector4& WallBB = wall.GetBoundingBox();
		if (Collied(clientBB, WallBB)) {
			return true;
		}
	}
	return false;
}

void ClientInfo::ChangeDirection(player_input& p_input)
{
	// ���� ��ȯ
	m_directionLock.lock();
	if (p_input.input == 0)
	{
		if (m_direction == Direction::UP) {
			m_direction = Direction::LEFT;
		}
		else if (m_direction == Direction::DOWN) {
			m_direction = Direction::RIGHT;
		}
		else if (m_direction == Direction::RIGHT) {
			m_direction = Direction::UP;
		}
		else if (m_direction == Direction::LEFT) {
			m_direction = Direction::DOWN;
		}
	}
	else if (p_input.input == 1)
	{
		if (m_direction == Direction::UP) {
			m_direction = Direction::RIGHT;
		}
		else if (m_direction == Direction::DOWN) {
			m_direction = Direction::LEFT;
		}
		else if (m_direction == Direction::RIGHT) {
			m_direction = Direction::DOWN;
		}
		else if (m_direction == Direction::LEFT) {
			m_direction = Direction::UP;
		}
	}
	m_directionLock.unlock();
}

void ClientInfo::SetNewPosition(start_game& s_game, float elapsedTIme, MapInfo& map)
{
	// Ŭ���̾�Ʈ���� �Է°��� ��� �޾ƿ��� ����
	// ���� �� ���� �ش� ���⿡ �浹�ϴ� ��ü �ִ��� üũ
	// �浹�� ��� �̵����� �ʰ�
	// �浹���� ���� ��� �̵�

	bool col = false;
	float x = m_pos_x, z = m_pos_z;
	float speed = 1 * elapsedTIme;
	
	m_directionLock.lock();
	Direction dir = m_direction;
	m_directionLock.unlock();

	if (dir == Direction::UP)
	{
		z = z + speed;
		// �浹üũ
		col = MapCollied(map);

		if (col == false)
			m_pos_z = z;
		else if(col == true)
			m_pos_z -= (2*speed);
	}
	else if (dir == Direction::DOWN)
	{
		z = z - speed;
		// �浹üũ
		col = MapCollied(map);

		if (col == false)
			m_pos_z = z;
		else if (col == true)
			m_pos_z += (2 * speed);
	}
	else if (dir == Direction::LEFT)
	{
		x = x + speed;
		// �浹üũ
		col = MapCollied(map);

		if (col == false)
			m_pos_x = x;
		else if (col == true)
			m_pos_x -= (2 * speed);
	}
	else if (dir == Direction::RIGHT)
	{
		x = x - speed;
		// �浹üũ
		col = MapCollied(map);

		if (col == false)
			m_pos_x = x;
		else if (col == true)
			m_pos_x += (2 * speed);
	}
}


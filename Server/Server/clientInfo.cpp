#include "clientInfo.h"

ClientInfo::ClientInfo()
	:	m_id(0),
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
	while (!m_recvMsg.IsEmpty())
	{
		MsgType msg_type = m_recvMsg.GetMsgType();

		switch (msg_type)
		{
		case MsgType::PLAYER_INPUT:
		{
			player_input pi{};
			m_recvMsg.Pop(reinterpret_cast<char*>(&pi), sizeof(player_input));
			cout << "size: " << pi.size << endl;
			cout << "type: " << (int)pi.type << endl;
			cout << "input: " << (int)pi.input << endl;
			cout << "x: " << pi.x << endl;
			cout << "z: " << pi.z << endl;
			break;
		}
		}
	}
}

void ClientInfo::CheckObjectsStatus()
{
}

bool ClientInfo::IsCollided(float x, float z, Direction dir, start_game &s_game)
{
	// ��, ���, ����, ��, �°ſ� ������ �浹
	int mapx = 0, mapz = 0;
	if (dir == Direction::UP || dir == Direction::RIGHT)
	{
		mapx = (int)x;
		mapz = (int)z;
	}
	else if (dir == Direction::DOWN)
	{
		mapx = (int)x;
		mapz = (int)z + 1;
	}
	else if (dir == Direction::LEFT)
	{
		mapx = (int)x + 1;
		mapz = (int)z;
	}

	if (s_game.mapinfo[mapz][mapx] == 2)		// �� �浹��
	{
		return true;
	}
	else if (s_game.mapinfo[mapz][mapx] == 4)		// ������ üũ��
	{
		return true;
	}


	return false;
}

void ClientInfo::GetPlayerInputInfo(player_input p_input)
{
	// ���� ��ȯ
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

	// ��ġ���� ����
	m_pos_x = p_input.x;
	m_pos_z = p_input.z;
}

void ClientInfo::GetNewPosition(start_game& s_game, float elapsedTIme)
{
	// Ŭ���̾�Ʈ���� �Է°��� ��� �޾ƿ��� ����
	// ���� �� ���� �ش� ���⿡ �浹�ϴ� ��ü �ִ��� üũ
	// �浹�� ��� �̵����� �ʰ�
	// �浹���� ���� ��� �̵�

	bool col = false;
	float x = m_pos_x, z = m_pos_z;
	float speed = 3 * elapsedTIme;

	if (m_direction == Direction::UP)
	{
		z = z - speed;
		// �浹üũ
		col = IsCollided(x, z, Direction::UP, s_game);

		if(col == false)
			m_pos_z = z;
	}
	else if (m_direction == Direction::DOWN)
	{
		z = z + speed;
		// �浹üũ
		col = IsCollided(x, z, Direction::DOWN, s_game);

		if (col == false)
			m_pos_z = z;
	}
	else if (m_direction == Direction::LEFT)
	{
		x = x + speed;
		// �浹üũ
		col = IsCollided(x, z, Direction::LEFT, s_game);

		if (col == false)
			m_pos_x = x;
	}
	else if (m_direction == Direction::RIGHT)
	{
		x = x - speed;
		// �浹üũ
		col = IsCollided(x, z, Direction::RIGHT, s_game);

		if (col == false)
			m_pos_x = x;
	}

	//return pair<float, float>();
}


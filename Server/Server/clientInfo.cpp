#include "clientInfo.h"

ClientInfo::ClientInfo()
	:	m_id(0),
		m_type(PlayerType::RUNNER),
		m_hp(0),
		m_pos_x(0.0f),
		m_pos_z(0.0f),
		m_boundingOffset(0.0f),
		m_sendMsg({}),
		m_direction(Direction::DOWN)
{
}

ClientInfo::~ClientInfo()
{
}

void ClientInfo::Init(SOCKET sck, int id)
{
	m_id = id;
	m_socket = sck;
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
			player_input pi{};
			m_recvMsg.Pop(reinterpret_cast<char*>(&pi), sizeof(player_input));
			ChangeDirection(pi);			
			break;
		}
		}
	}
}

bool ClientInfo::IsCollided(float x, float z, Direction dir, start_game &s_game)
{
	int mapx = 0, mapz = 0;
	if (dir == Direction::UP || dir == Direction::RIGHT)
	{
		mapx = (int)x;
		mapz = (int)z;
	}
	else if (dir == Direction::DOWN)
	{
		mapx = (int)x;
		mapz = (int)(z + 1);
	}
	else if (dir == Direction::LEFT)
	{
		mapx = (int)(x + 1);
		mapz = (int)z;
	}

	if (s_game.mapinfo[mapz][mapx] == 2)		// 벽 충돌시
	{
		return true;
	}
	else if (s_game.mapinfo[mapz][mapx] == 4)		// 최종문 체크시
	{
		return true;
	}
	return false;
}

void ClientInfo::ChangeDirection(player_input& p_input)
{
	// 방향 전환
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

void ClientInfo::SetNewPosition(start_game& s_game, float elapsedTIme)
{
	// 클라이언트에서 입력값을 계속 받아오고 있음
	// 받을 때 마다 해당 방향에 충돌하는 물체 있는지 체크
	// 충돌할 경우 이동하지 않고
	// 충돌하지 않을 경우 이동

	bool col = false;
	float x = m_pos_x, z = m_pos_z;
	float speed = 1 * elapsedTIme;
	
	m_directionLock.lock();
	Direction dir = m_direction;
	m_directionLock.unlock();

	if (dir == Direction::UP)
	{
		z = z + speed;
		// 충돌체크
		col = IsCollided(x, z, Direction::UP, s_game);

		if(col == false)
			m_pos_z = z;
	}
	else if (dir == Direction::DOWN)
	{
		z = z - speed;
		// 충돌체크
		col = IsCollided(x, z, Direction::DOWN, s_game);

		if (col == false)
			m_pos_z = z;
	}
	else if (dir == Direction::LEFT)
	{
		x = x - speed;
		// 충돌체크
		col = IsCollided(x, z, Direction::LEFT, s_game);

		if (col == false)
			m_pos_x = x;
	}
	else if (dir == Direction::RIGHT)
	{
		x = x + speed;
		// 충돌체크
		col = IsCollided(x, z, Direction::RIGHT, s_game);

		if (col == false)
			m_pos_x = x;
	}
}


#include "clientInfo.h"

ClientInfo::ClientInfo()
	: m_id(0),
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
	m_sendLock.lock();
	Send(m_sendMsg);
	m_sendLock.unlock();
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
			//std::cout << "Received input message\n";
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
	if (a.MinZ > b.MaxZ || b.MinZ > a.MaxZ)
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

bool ClientInfo::IsCollied(int r, int c, start_game& s_game)
{
	if (s_game.mapinfo[r][c] == '2') {
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

	bool collied = false;
	float speed = 8 * elapsedTIme;

	int row = (m_pos_z) / 7.5f;
	int col = (m_pos_x) / 7.5f;
	float row_result = (m_pos_z/7.5f) - row;
	float col_result = (m_pos_x/7.5f) - col;
	if (row_result >= 0.5f)
		row += 1;
	if (col_result >= 0.5f)
		col += 1;

	m_directionLock.lock();
	Direction dir = m_direction;
	m_directionLock.unlock();

	if (dir == Direction::DOWN)
	{
		if (row_result >= 0.5f)
			collied = IsCollied(row, col, s_game);
		else
			collied = IsCollied(row+1, col, s_game);
		if (collied == false)
			m_pos_z += speed;
	}
	else if (dir == Direction::UP)
	{
		if (row_result >= 0.5f)
			collied = IsCollied(row-1, col, s_game);
		else
			collied = IsCollied(row, col, s_game);
		if (collied == false)
			m_pos_z -= speed;
	}
	else if (dir == Direction::RIGHT)
	{
		if (col_result >= 0.5f)
			collied = IsCollied(row, col, s_game);
		else
			collied = IsCollied(row, col+1, s_game);
		if (collied == false)
			m_pos_x += speed;
	}
	else if (dir == Direction::LEFT)
	{
		if (col_result >= 0.5f)
			collied = IsCollied(row, col - 1, s_game);
		else
			collied = IsCollied(row, col, s_game);
		if (collied == false)
			m_pos_x -= speed;
	}
}


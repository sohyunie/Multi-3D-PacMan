#include "clientInfo.h"

Timer ClientInfo::m_timer;

ClientInfo::ClientInfo()
	:	m_id(0),
		m_type(PlayerType::RUNNER),
		m_hp(0),
		m_pos_x(0.0f),
		m_pos_z(0.0f),
		m_boundingOffset(0.0f),
		m_sendMsg({}),
		m_direction(Direction::UP),
		m_input()
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

void ClientInfo::CheckObjectsStatus()
{
}

void ClientInfo::IsCollided(Vector4& a, Vector4& b)
{
}

bool ClientInfo::IsCollided(float x, float z, Direction dir)
{
	// 벽, 비드, 열쇠, 문, 태거와 러너의 충돌
	int mapx, mapz;
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

	if (MAP[mapz][mapx] == 0)				// 비드 충돌시
	{

	}
	else if (MAP[mapz][mapx] == 1)		// 키 충돌시
	{

	}
	else if (MAP[mapz][mapx] == 2)		// 벽 충돌시
	{
		return true;
	}
	else if (MAP[mapz][mapx] == 3)		// 플레이어 시작 위치(아무 변화 없음)
	{

	}
	else if (MAP[mapz][mapx] == 4)		// 최종문 체크시
	{
		return true;
	}


	return false;
}

void ClientInfo::GetPlayerInputInfo(player_input p_input)
{
	// 방향 전환
	if (p_input.input == Direction::LEFT)
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
	else if (p_input.input == Direction::RIGHT)
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

	// 위치정보 저장
	m_pos_x = p_input.x;
	m_pos_z = p_input.z;
}

void ClientInfo::GetNewPosition()
{
	// 클라이언트에서 입력값을 계속 받아오고 있음
	// 받을 때 마다 해당 방향에 충돌하는 물체 있는지 체크
	// 충돌할 경우 이동하지 않고
	// 충돌하지 않을 경우 이동

	bool col = false;
	float x = m_pos_x, z = m_pos_z;
	float speed = 3 * m_timer.GetElapsedTime();

	if (m_direction == Direction::UP)
	{
		z = z - speed;
		// 충돌체크
		col = IsCollided(x, z, Direction::UP);

		if(col == false)
			m_pos_z = z;
	}
	else if (m_direction == Direction::DOWN)
	{
		z = z + speed;
		// 충돌체크
		col = IsCollided(x, z, Direction::DOWN);

		if (col == false)
			m_pos_z = z;
	}
	else if (m_direction == Direction::LEFT)
	{
		x = x + speed;
		// 충돌체크
		col = IsCollided(x, z, Direction::LEFT);

		if (col == false)
			m_pos_x = x;
	}
	else if (m_direction == Direction::RIGHT)
	{
		x = x - speed;
		// 충돌체크
		col = IsCollided(x, z, Direction::RIGHT);

		if (col == false)
			m_pos_x = x;
	}

	//return pair<float, float>();
}

void ClientInfo::CreateLoginOkAndMapInfoMsg(start_game& s_game)
{
	m_sendMsg.Push(reinterpret_cast<char*>(&s_game), sizeof(s_game));
	ClientInfo::Send();
}

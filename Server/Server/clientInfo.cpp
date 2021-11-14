#include "clientInfo.h"

Timer ClientInfo::m_timer;

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
	Vector4 vec;
	vec.MaxX = m_pos_x + 0.5;
	vec.MaxZ = m_pos_z + 0.5;
	vec.MinX = m_pos_x - 0.5;
	vec.MinZ = m_pos_z - 0.5;

	return vec;
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
	// ��, ���, ����, ��, �°ſ� ������ �浹
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

	if (MAP[mapz][mapx] == 0)				// ��� �浹��
	{

	}
	else if (MAP[mapz][mapx] == 1)		// Ű �浹��
	{

	}
	else if (MAP[mapz][mapx] == 2)		// �� �浹��
	{
		return true;
	}
	else if (MAP[mapz][mapx] == 3)		// �÷��̾� ���� ��ġ(�ƹ� ��ȭ ����)
	{

	}
	else if (MAP[mapz][mapx] == 4)		// ������ üũ��
	{
		return true;
	}


	return false;
}

pair<float, float> ClientInfo::GetNewPosition(player_input p_input)
{
	// Ŭ���̾�Ʈ���� �Է°��� ��� �޾ƿ��� ����
	// ���� �� ���� �ش� ���⿡ �浹�ϴ� ��ü �ִ��� üũ
	// �浹�� ��� �̵����� �ʰ�
	// �浹���� ���� ��� �̵�

	bool col = false;
	float x, z;

	if (p_input.input == Direction::UP)
	{
		z = p_input.z - m_timer.GetElapsedTime();
		// �浹üũ
		col = IsCollided(p_input.x, z, Direction::UP);

		if(col == false)
			p_input.z = z;
	}
	else if (p_input.input == Direction::DOWN)
	{
		z = p_input.z + m_timer.GetElapsedTime();
		// �浹üũ
		col = IsCollided(p_input.x, z, Direction::DOWN);

		if (col == false)
			p_input.z = z;
	}
	else if (p_input.input == Direction::LEFT)
	{
		x = p_input.x + m_timer.GetElapsedTime();
		// �浹üũ
		col = IsCollided(x, p_input.z, Direction::LEFT);

		if (col == false)
			p_input.x = x;
	}
	else if (p_input.input == Direction::RIGHT)
	{
		x = p_input.x - m_timer.GetElapsedTime();
		// �浹üũ
		col = IsCollided(x, p_input.z, Direction::RIGHT);

		if (col == false)
			p_input.x = x;
	}

	return pair<float, float>();
}

void ClientInfo::CreateLoginOkAndMapInfoMsg(start_game& s_game)
{
	m_sendMsg.Push(reinterpret_cast<char*>(&s_game), sizeof(s_game));
	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 30; ++j) {
			MAP[i][j] = s_game.mapinfo[i][j];
		}
	}
}

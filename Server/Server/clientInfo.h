#pragma once

#include "common.h"

class ClientInfo : public Socket
{
public:
	ClientInfo();
	virtual ~ClientInfo();

public:
	void Init(SOCKET sck, int id);

	void SendMsg();

	void ProcessMessage();
	bool Collied(const Vector4& a, const Vector4& b);
	bool IsCollided(float x, float z, Direction dir, start_game& s_game, MapInfo& map);
	
	void ChangeDirection(player_input& p_input);
	void SetNewPosition(start_game& s_game, float elapsedTime, MapInfo& map);
	Vector4 GetBoundingBox();

public:
	char m_id;
	PlayerType m_type;	
	int m_hp;
	float m_pos_x;
	float m_pos_z;
	float m_boundingOffset;
	bool m_active;

	std::chrono::high_resolution_clock::time_point hit_time;

	Message m_sendMsg;
	mutex m_directionLock;
	Direction m_direction;
};
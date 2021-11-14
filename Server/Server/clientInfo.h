#pragma once

#include "common.h"

class ClientInfo : public Socket
{
public:
	ClientInfo();
	ClientInfo(SOCKET sck, int id);
	virtual ~ClientInfo();

public:
	void Send();
	
	void ProcessMessage();
	void CheckObjectsStatus();
	void IsCollided(Vector4& a, Vector4& b);
	bool IsCollided(float x, float z, Direction dir);
	void CreateLoginOkAndMapInfoMsg(start_game& s_game);

	pair<float, float> GetNewPosition(player_input p_input);
	Vector4 GetBoundingBox();

	static Timer m_timer;
private:
	int m_id;
	PlayerType m_type;
	
	int m_hp;

	float m_pos_x;
	float m_pos_z;

	float m_boundingOffset;
	Message m_sendMsg;
	char temp_buffer[1024]; // Test
	Direction m_direction;

	player_input m_input;
	char MAP[30][30];
};
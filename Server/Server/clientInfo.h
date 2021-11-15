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
	void CheckObjectsStatus(MapInfo& map);
	bool IsCollided(const Vector4& a, const Vector4& b);
	void CreateLoginOkAndMapInfoMsg(start_game& s_game);
	
	pair<float, float> GetNewPosition();
	Vector4 GetBoundingBox();

	
public:
	int m_id;
	PlayerType m_type;	
	int m_hp;
	float m_pos_x;
	float m_pos_z;
	float m_boundingOffset;

private:
	Message m_sendMsg;
	char temp_buffer[1024]; // Test
	Direction m_direction;
};
#pragma once

#include "common.h"

class ClientInfo : public Socket
{
public:
	ClientInfo();
	virtual ~ClientInfo();

public:
	Vector4 GetBoundingBox();
	void ProcessPacket(char* packets);
	void CheckObjectsStatus();
	void IsCollided(Vector4& a, Vector4& b);
	pair<float, float> GetNewPosition();
	void CreateLoginOkAndMapInfoMsg();

private:
	int m_id;
	PlayerType m_type;
	
	int m_hp;

	float m_pos_x;
	float m_pos_z;

	float m_boundingOffset;
	Message m_sendMsg;
	Direction m_direction;
};
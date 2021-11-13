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
<<<<<<< HEAD
	void CreateLoginOkAndMapInfoMsg(start_game s_game);
	
=======
	void CreateLoginOkAndMapInfoMsg();

>>>>>>> 6a2cfd896f3188109263f49570112d643876ecf6
	pair<float, float> GetNewPosition();
	Vector4 GetBoundingBox();

	
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
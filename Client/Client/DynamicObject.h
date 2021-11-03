#pragma once
#include "Object.h"

class DynamicObject : public Object
{
private:

public:
	DynamicObject() {

	}

	void SetPlayerPos(Vector3 playerPos) { this->position = playerPos; }
	void SetPlayerPos(glm::vec3 playerPos) { this->position = playerPos; }

	DIRECTION progressDirection = DIRECTION::DIR_NONE;
	DIRECTION priorDirection = DIRECTION::DIR_NONE;

	int board_i = 3;
	int board_j = 2;
	bool isArrived = true; // 플레이어가 다른 칸에 도착한 그 순간! true, 그 외에는 항상 
	float acc = 0.f; // 이동 amount
	float accDir = 0.f; // 회전 amount
	DIRECTION newDirection = DIRECTION::DIR_NONE;
	Vector3 playerPriorDirVec3;
	Vector3 playerDirVec3;
	bool isNewMapCollision = false;
	int temp_i = 0;
	int temp_j = 0;

	bool isChangeCameraDir = false;
	Vector3 dir = Vector3(0,0,1);
};
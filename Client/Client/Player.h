#pragma once
#include "DynamicObject.h"
class Player : public DynamicObject
{
private:

public:
	Player();

	virtual void DrawObject(GLuint s_program) override;

	Vector3 GetPlayerPos() { return this->position; }
	void InitPlayerPos(int i, int j, Vector3 pos);
	void ComputePos(float deltaMove, float lx, float lz);
	float angle = 0.0f;
	float deltaAngle = 0.0f;
	float deltaMove = 0.0f;
	float hp = 100.0f;
};



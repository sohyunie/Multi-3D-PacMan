#pragma once
#include "DynamicObject.h"

class Ghost : public DynamicObject
{
private:
	bool isActive = true;
	int id;
	float rotateAngle = 0.0f;
public:
	Ghost();
	Ghost(int i, int j, Vector3 pos, int id);
	void SetRandomDirection();
	virtual void DrawObject(GLuint s_program) override;
	void MakeGhost(Ghost ghost);
	bool GetIsActive() { return this->isActive; }
	void SetIsActive(bool isOn) { this->isActive = isOn; }
	int GetID() { return this->id; }


	/*void SetGhostColor(glm::vec3 ghostColor) {
		this->color = ghostColor;
	}*/
};

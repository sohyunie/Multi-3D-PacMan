#pragma once
#include "StaticObject.h"
class PowerBead : public StaticObject
{
private:

public:
	PowerBead();
	PowerBead(Vector3 position);
	virtual void DrawObject(GLuint s_program) override;
};


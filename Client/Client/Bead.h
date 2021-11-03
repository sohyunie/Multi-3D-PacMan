#pragma once
#include "StaticObject.h"
class Bead : public StaticObject
{
private:

public:
	Bead();
	Bead(Vector3 pos);
	virtual void DrawObject(GLuint s_program) override;
};

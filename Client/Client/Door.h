#pragma once
#include "StaticObject.h"

class Door : public StaticObject
{
private:
public:
	Door();
	Door(Vector3 pos);

	virtual void DrawObject(GLuint s_program) override;
};

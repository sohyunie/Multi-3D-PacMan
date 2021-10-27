#pragma once
#include "StaticObject.h"

class Block : public StaticObject
{
private:
public:
	Block();
	Block(Vector3 pos);

	virtual void DrawObject(GLuint s_program) override;
};

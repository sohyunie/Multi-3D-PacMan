#pragma once
#include "StaticObject.h"

class Bottom : public StaticObject
{
private:
public:
	Bottom();
	Bottom(Vector3 pos);

	virtual void DrawObject(GLuint s_program) override;
};

#pragma once
#include "Object.h"
class StaticObject : public Object
{
private:

public:
	StaticObject() {
		this->type = ObjectType::ROAD;
	}

	StaticObject(Vector3 pos) {
		this->type = ObjectType::ROAD;
		this->position = pos;
		this->boundingOffset = 1.5f;//1.8;
	}
	virtual void DrawObject(GLuint s_program) override {
		//cout << "DrawObject : StaticObject" << endl;
	}
};
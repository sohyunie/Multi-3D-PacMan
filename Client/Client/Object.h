#pragma once
#ifndef OBJECT_H
#define OBJECT_H

class Object
{
private:

public:
	Object() { }
	virtual void DrawObject(GLuint s_program);
	bool CollisionCheck(Object type);
	Vector4 GetBoundingBox();
	Vector3 GetPosition() { return position; }
	ObjectType GetType() { return type; }
	ObjectType SetType(ObjectType type) { this->type = type; }
	void SetPosition(Vector3 position) { this->position = position; }
protected:
	Vector3 position;
	Vector3 color;
	Vector3 scale;
	Vector3 rotate;
	ObjectType type;
	float boundingOffset; // bbox¿ß«‘!
};

#endif
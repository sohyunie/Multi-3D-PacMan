#pragma once
class StaticObject;

class MapLoader
{
private:

public:
	MapLoader();
	MapLoader(int map);
	StaticObject* boardShape[MAP_SIZE][MAP_SIZE];
	GLvoid Loadfile(int map);
	GLvoid DrawMap(GLuint s_program);
	GLvoid DrawMapSecondFloor(GLuint s_program);
};


#pragma once
class StaticObject;

class MapLoader
{
private:

public:
	MapLoader();
	MapLoader(char mapArray[][30]);
	StaticObject* boardShape[MAP_SIZE][MAP_SIZE];
	GLvoid DrawMap(GLuint s_program);
};


#pragma once
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(disable:4996)
#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <time.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <mmsystem.h>
#include <Digitalv.h>
#include <gl/glew.h> 
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tchar.h>
#include <thread>
#include "Dependencies/glew.h"
#include "Dependencies/freeglut.h"

#pragma comment (lib, "winmm.lib")

// 네트워크 헤더
#include "Socket.h"
#include "Protocol.h"
#include "Exception.h"
#include "Message.h"

class Ghost;

using namespace std;
const int MAX_VAO_TYPE = 10;

static MCI_OPEN_PARMS openBgm;
static MCI_PLAY_PARMS playBgm;
static MCI_OPEN_PARMS openFxSound;
static MCI_PLAY_PARMS playFxSound;

#define WINDOW_WITDH	800
#define WINDOW_HEIGHT	600

#define CAMERA_POS		glm::vec3(0.0, 0.0, 50.0)
#define OBJECT_COLOR	glm::vec3(1.0, 1.0, 1.0)
#define LIGHT_AMBIENT	glm::vec3(0.1, 0.1, 0.1)
#define LIGHT_COLOR		glm::vec3(1.0, 1.0, 1.0)

#define DOOR_FILE_NAME		"doors.obj"
#define BEAD_FILE_NAME	"bead.obj"
#define KEY_ITEM_FILE_NAME	"key.obj"
#define CUBE_FILE_NAME	"block.obj"

#define SOUND_FILE_NAME_INGAME	"bgm_InGame.wav"  // 배경음악
#define SOUND_FILE_NAME_BEAD	"bgm_Bead.wav"
#define SOUND_FILE_NAME_LOBBY	"bgm_Lobby.wav"
//#define SOUND_FILE_NAME_GAMEOVER	"../sound/backgroundmusic.wav"
//#define SOUND_FILE_NAME_CLEAR	"../bgm_Clear.wav"

#define MAP_SIZE 30
#define MAX_TIME 120
#define GHOST_SPAWN_TIME 3 * 60 * 60
#define POWER_BEAD_TIME 2.5 * 60 * 60
#define COLLISION_TIME 0.5 * 60 * 60

#define GHOST_SPEED 0.002
#define NORMAL_SPEED 0.001
#define POWER_SPEED 0.004
struct ObjData {
	float* vPosData;	// 값이 하나만 있어도 되는 건 그냥 변수로/ 아니고 여러개가 나열되고 필요한 것들은 *로 받는다.
	float* vNormalData;
	float* vTextureCoordinateData;
	int* indexData;
	int vertexCount;
	int indexCount;
};

struct Vector3 {
	float x;
	float y;
	float z;

	Vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3(glm::vec3 vec3) {
		this->x = vec3.x;
		this->y = vec3.y;
		this->z = vec3.z;
	}

	bool operator== (Vector3 value) {
		float sameX = abs(this->x - value.x);
		float sameY = abs(this->y - value.y);
		float sameZ = abs(this->z - value.z);
		float check = sameX + sameY + sameZ;
		bool result = check < 1.0f; // 오차범위
		// cout << check << endl;
		return result;
	}

	Vector3 operator+ (Vector3 value) {
		return Vector3(x + value.x, y + value.y, z + value.z);
	}

	Vector3 operator- (Vector3 value) {
		return Vector3(x - value.x, y - value.y, z - value.z);
	}

	Vector3 operator* (float value) {
		return Vector3(x * value, y * value, z * value);
	}

	glm::vec3 GetGlmVec3() {
		return glm::vec3(this->x, this->y, this->z);
	}

	glm::vec3 GetGlmVec3(float gap) {
		return glm::vec3(this->x, this->y + gap, this->z);
	}

	Vector3() {}
};

// 충돌처리 위한 벡터 구조체 
struct CVector4 {
    float minX;
    float minZ;
    float maxX;
    float maxZ;

    CVector4() {}

    CVector4(float minX, float minZ, float maxX, float maxZ) {
        this->minX = minX;
        this->minZ = minZ;
        this->maxX = maxX;
        this->maxZ = maxZ;
    }
};

enum class TextureType {
	LOBBY,
	GAMEOVER,
	CLEAR,
	INGAME
};

// Map 타입
enum BOARD_TYPE {
	BEAD_ITEM,
	KEY_ITEM,
	WALL_0,
	NONE,
	INIT_PLAYER_1,
	INIT_PLAYER_2,
	INIT_GHOST_POS
};

enum class GAMESTATE {
	NONE,
	LOBBY,
	INGAME,
	GAMEOVER,
	CLEAR,
};
struct Shape {
	BOARD_TYPE type;
	Vector3 color;
	Vector3 scale;
	Vector3 pos;
	Vector3 dir;
	float boundingOffset;
	bool isAlive;
	int hitCount = 3;
	float speed;
};

struct GhostCollisionData {
	Ghost* ghost = nullptr;
	float time;

	GhostCollisionData(Ghost* ghost, float time) {
		this->ghost = ghost;
		this->time = time;
	}

	//bool operator== (GhostCollisionData value) {
	//	bool isSame = false;
	//	if (value.ghost != nullptr && ghost != nullptr)
	//		isSame = ghost->GetID() == value.ghost->GetID();
	//	return isSame;
	//}
};
struct BasePacket
{
	short size;
	MsgType type;
};
//float deltaTime = 0.0f;

const float blockVertex[6][6][2][3] = {
	//--- 버텍스 속성: 좌표값(FragPos), 노말값 (Normal)

	{
		// Back
		{	{-0.5f, -0.5f, -0.5f},	{0.0f, 0.0f, -1.0f}	},
		{	{ 0.5f, -0.5f, -0.5f},	{0.0f, 0.0f, -1.0f}	},
		{	{ 0.5f,  0.5f, -0.5f},	{0.0f, 0.0f, -1.0f}	},
		{	{ 0.5f,  0.5f, -0.5f},	{0.0f, 0.0f, -1.0f}	},
		{	{-0.5f,  0.5f, -0.5f},	{0.0f, 0.0f, -1.0f}	},
		{	{-0.5f, -0.5f, -0.5f},	{0.0f, 0.0f, -1.0f}	}
	},

	{
		// Front
		{	{-0.5f, -0.5f, 0.5f},	{0.0f, 0.0f, 1.0f}	},
		{	{ 0.5f, -0.5f, 0.5f},	{0.0f, 0.0f, 1.0f}	},
		{	{ 0.5f,  0.5f, 0.5f},	{0.0f, 0.0f, 1.0f}	},
		{	{ 0.5f,  0.5f, 0.5f},	{0.0f, 0.0f, 1.0f}	},
		{	{-0.5f,  0.5f, 0.5f},	{0.0f, 0.0f, 1.0f}	},
		{	{-0.5f, -0.5f, 0.5f},	{0.0f, 0.0f, 1.0f}	}
	},

	{
		// Left
		{	{-0.5f,  0.5f,  0.5f},	{-1.0f, 0.0f, 0.0f}	},
		{	{-0.5f,  0.5f, -0.5f},	{-1.0f, 0.0f, 0.0f}	},
		{	{-0.5f, -0.5f, -0.5f},	{-1.0f, 0.0f, 0.0f}	},
		{	{-0.5f, -0.5f, -0.5f},	{-1.0f, 0.0f, 0.0f}	},
		{	{-0.5f, -0.5f,  0.5f},	{-1.0f, 0.0f, 0.0f}	},
		{	{-0.5f,  0.5f,  0.5f},	{-1.0f, 0.0f, 0.0f}	}
	},

	{
		// Right
		{	{0.5f,  0.5f,  0.5f},	{1.0f, 0.0f, 0.0f}	},
		{	{0.5f,  0.5f, -0.5f},	{1.0f, 0.0f, 0.0f}	},
		{	{0.5f, -0.5f, -0.5f},	{1.0f, 0.0f, 0.0f}	},
		{	{0.5f, -0.5f, -0.5f},	{1.0f, 0.0f, 0.0f}	},
		{	{0.5f, -0.5f,  0.5f},	{1.0f, 0.0f, 0.0f}	},
		{	{0.5f,  0.5f,  0.5f},	{1.0f, 0.0f, 0.0f}	}
	},
	{
		// Bottom
		{	{-0.5f, -0.5f, -0.5f},	{0.0f, -1.0f, 0.0f}	},
		{	{ 0.5f, -0.5f, -0.5f},	{0.0f, -1.0f, 0.0f} },
		{	{ 0.5f, -0.5f,  0.5f},	{0.0f, -1.0f, 0.0f}	},
		{	{ 0.5f, -0.5f,  0.5f},	{0.0f, -1.0f, 0.0f}	},
		{	{-0.5f, -0.5f,  0.5f},	{0.0f, -1.0f, 0.0f}	},
		{	{-0.5f, -0.5f, -0.5f},	{0.0f, -1.0f, 0.0f}	}
	},

	{
		// Top
		{	{-0.5f, 0.5f, -0.5f},	{0.0f, 1.0f, 0.0f}	},
		{	{ 0.5f, 0.5f, -0.5f},	{0.0f, 1.0f, 0.0f}	},
		{	{ 0.5f, 0.5f,  0.5f},	{0.0f, 1.0f, 0.0f}	},
		{	{ 0.5f, 0.5f,  0.5f},	{0.0f, 1.0f, 0.0f}	},
		{	{-0.5f, 0.5f,  0.5f},	{0.0f, 1.0f, 0.0f}	},
		{	{-0.5f, 0.5f, -0.5f},	{0.0f, 1.0f, 0.0f}	}
	}
};

const float Background[] = {
	-1.0,1.0,-1.0, 0.0,1.0,0.0, 1.0,1.0,
	-1.0,-1.0,-1.0, 0.0,1.0,0.0, 1.0,0.0,
	1.0,1.0,-1.0, 0.0,1.0,0.0, 0.0,1.0,

	-1.0,-1.0,-1.0, 0.0,1.0,0.0, 1.0,0.0,
	1.0,-1.0,-1.0, 0.0,1.0,0.0, 0.0,0.0,
	1.0,1.0,-1.0, 0.0,1.0,0.0, 0.0,1.0
};

static Vector3 lightPos = Vector3(0, 20, 0);
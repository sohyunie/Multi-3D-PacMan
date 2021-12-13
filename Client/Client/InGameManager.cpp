#include "Standard.h"
#include "InGameManager.h"
#include "ReadObj.h"
#include "Ghost.h"
#include "Block.h"
#include "Player.h"
#include "Bead.h"
#include "PowerBead.h"
#include "DynamicObject.h"
#include "MapLoader.h"
#include "InGameUI.h"
#include "Bottom.h"
#include "ReadObj.h"
#include "StartSceneUI.h"
#include "EndingScene.h"
#include "stb_image.h"
#include "NetworkManager.h"
//#include "Object.h"

GLchar* vertexsource, * fragmentsource; // 소스코드 저장 변수
GLuint vertexShader, fragmentShader; // 세이더 객체
InGameManager* InGameManager::instance = nullptr;	//  static은 초기화가 필요 InGameManager*: 변수 타입/ InGameManager::instance : 인게임매니저 안에 private선언되어 있는 instance를 가리킨다. 

default_random_engine dre((size_t)time(NULL));
normal_distribution <float>uidGhostLocation{ -20.0,20.0 };

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	errno_t erron = fopen_s(&fptr, file, "rb"); // Open file for reading
	if (erron != 0) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}

void make_vertexShaders()
{
	char* vertexsource = filetobuf("vertex.glsl");

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexsource, 0);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
	}
}


void make_fragmentShaders()
{
	char* fragmentsource = filetobuf("fragment.glsl");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentsource, 0);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader error\n" << errorLog << endl;
	}
}

void InGameManager::CalculateTime() {
	this->inGameTime += this->deltaTime;
}


void InGameManager::CameraSetting(bool isFps) {
	if (this->state == GAMESTATE::INGAME) {
		if (false) {
			Vector3 dir = Vector3(this->player->GetPosition().x + this->player->dir.x, -1, this->player->GetPosition().z + this->player->dir.z);
			this->cameraDirection = dir.GetGlmVec3();
			this->cameraPos = this->player->GetPosition().GetGlmVec3();
			this->cameraPos.y += 1;
			//this->cameraPos.y += 1;
		}
		else {
			Vector3 dir = Vector3(this->player->GetPosition().x + this->player->dir.x, 0, this->player->GetPosition().z + this->player->dir.z);
			dir.y = 5;
			this->cameraDirection = dir.GetGlmVec3();
			//this->cameraDirection.y += 5;
			Vector3 newCameraPos = this->player->GetPosition().GetGlmVec3();
			newCameraPos.y += 50;
			this->cameraPos = newCameraPos.GetGlmVec3();
		}
	}
}

GLuint s_program;

GLvoid InGameManager::InitBuffer() {
	cout << "InitBuffer InGameManager" << endl;
	// PLAYER
	glGenVertexArrays(1, &this->VAO[(int)ObjectType::PLAYER]);
	glBindVertexArray(this->VAO[(int)ObjectType::PLAYER]);
	glGenBuffers(3, this->VBO[(int)ObjectType::PLAYER]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::PLAYER][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::PLAYER]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::PLAYER]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::PLAYER][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::PLAYER]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::PLAYER]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::PLAYER][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::PLAYER]->vertexCount * sizeof(float) * 2, this->objData[(int)ObjectType::PLAYER]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[(int)ObjectType::PLAYER]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[(int)ObjectType::PLAYER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[(int)ObjectType::PLAYER]->indexCount * sizeof(int), this->objData[(int)ObjectType::PLAYER]->indexData, GL_STATIC_DRAW);


	// GHOST
	glGenVertexArrays(1, &this->VAO[(int)ObjectType::DOOR]);
	glBindVertexArray(this->VAO[(int)ObjectType::DOOR]);
	glGenBuffers(3, this->VBO[(int)ObjectType::DOOR]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::DOOR][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::DOOR]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::DOOR]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::DOOR][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::DOOR]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::DOOR]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::DOOR][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::DOOR]->vertexCount * sizeof(float) * 2, this->objData[(int)ObjectType::DOOR]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[(int)ObjectType::DOOR]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[(int)ObjectType::DOOR]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[(int)ObjectType::DOOR]->indexCount * sizeof(int), this->objData[(int)ObjectType::DOOR]->indexData, GL_STATIC_DRAW);


	// BLOCK
	glGenVertexArrays(1, &this->VAO[(int)ObjectType::WALL]);
	glBindVertexArray(this->VAO[(int)ObjectType::WALL]);
	glGenBuffers(3, this->VBO[(int)ObjectType::WALL]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::WALL][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::WALL]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::WALL]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::WALL][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::WALL]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::WALL]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::WALL][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::WALL]->vertexCount * sizeof(float) * 2, this->objData[(int)ObjectType::WALL]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[(int)ObjectType::WALL]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[(int)ObjectType::WALL]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[(int)ObjectType::WALL]->indexCount * sizeof(int), this->objData[(int)ObjectType::WALL]->indexData, GL_STATIC_DRAW);


	// BEAD
	glGenVertexArrays(1, &this->VAO[(int)ObjectType::BEAD]);
	glBindVertexArray(this->VAO[(int)ObjectType::BEAD]);
	glGenBuffers(3, this->VBO[(int)ObjectType::BEAD]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::BEAD][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::BEAD]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::BEAD]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::BEAD][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::BEAD]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::BEAD]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::BEAD][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::BEAD]->vertexCount * sizeof(float) * 2, this->objData[(int)ObjectType::BEAD]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[(int)ObjectType::BEAD]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[(int)ObjectType::BEAD]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[(int)ObjectType::BEAD]->indexCount * sizeof(int), this->objData[(int)ObjectType::BEAD]->indexData, GL_STATIC_DRAW);



	// POWERBEAD
	glGenVertexArrays(1, &this->VAO[(int)ObjectType::POWERBEAD]);
	glBindVertexArray(this->VAO[(int)ObjectType::POWERBEAD]);
	glGenBuffers(3, this->VBO[(int)ObjectType::POWERBEAD]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::POWERBEAD][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::POWERBEAD]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::POWERBEAD]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::POWERBEAD][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::POWERBEAD]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::POWERBEAD]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::POWERBEAD][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::POWERBEAD]->vertexCount * sizeof(float) * 2, this->objData[(int)ObjectType::POWERBEAD]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[(int)ObjectType::POWERBEAD]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[(int)ObjectType::POWERBEAD]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[(int)ObjectType::POWERBEAD]->indexCount * sizeof(int), this->objData[(int)ObjectType::POWERBEAD]->indexData, GL_STATIC_DRAW);


	// BOTTOM
	glGenVertexArrays(1, &this->VAO[(int)ObjectType::BOTTOM]);
	glBindVertexArray(this->VAO[(int)ObjectType::BOTTOM]);
	glGenBuffers(3, this->VBO[(int)ObjectType::BOTTOM]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::BOTTOM][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::BOTTOM]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::BOTTOM]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::BOTTOM][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::BOTTOM]->vertexCount * sizeof(float) * 3, this->objData[(int)ObjectType::BOTTOM]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[(int)ObjectType::BOTTOM][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[(int)ObjectType::BOTTOM]->vertexCount * sizeof(float) * 2, this->objData[(int)ObjectType::BOTTOM]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[(int)ObjectType::BOTTOM]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[(int)ObjectType::BOTTOM]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[(int)ObjectType::BOTTOM]->indexCount * sizeof(int), this->objData[(int)ObjectType::BOTTOM]->indexData, GL_STATIC_DRAW);


	//Texture
	//glGenVertexArrays(1, &this->VAO[TEXTURE]);
	//glGenBuffers(3, this->VBO[TEXTURE]);
	//glBindVertexArray(VAO[TEXTURE]);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO[TEXTURE][0]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Background), Background, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //--- 위치 속성
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말값 속성
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //--- 텍스처 좌표 속성
	//glEnableVertexAttribArray(2);
}

void InGameManager::InitTexture() {
	//Texture
	glGenVertexArrays(1, &this->VAO[(int)ObjectType::TEXTURE]);
	glBindVertexArray(VAO[(int)ObjectType::TEXTURE]);
	glGenBuffers(3, this->VBO[(int)ObjectType::TEXTURE]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[(int)ObjectType::TEXTURE][0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Background), Background, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //--- 위치 속성
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //--- 노말값 속성
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //--- 텍스처 좌표 속성
	glEnableVertexAttribArray(2);

	int width[10], height[10], nrChannels[10];
	stbi_set_flip_vertically_on_load(true);

	glGenTextures(1, &this->texture[(int)TextureType::LOBBY]);
	glBindTexture(GL_TEXTURE_2D, this->texture[(int)TextureType::LOBBY]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data1 = stbi_load("lobby.png", &width[(int)TextureType::LOBBY], &height[(int)TextureType::LOBBY], &nrChannels[(int)TextureType::LOBBY], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[(int)TextureType::LOBBY], height[(int)TextureType::LOBBY], 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data1);

	glGenTextures(1, &this->texture[(int)TextureType::GAMEOVER]);
	glBindTexture(GL_TEXTURE_2D, this->texture[(int)TextureType::GAMEOVER]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data2 = stbi_load("gameover.png", &width[(int)TextureType::GAMEOVER], &height[(int)TextureType::GAMEOVER], &nrChannels[(int)TextureType::GAMEOVER], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[(int)TextureType::GAMEOVER], height[(int)TextureType::GAMEOVER], 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data2);

	glGenTextures(1, &this->texture[(int)TextureType::CLEAR]);
	glBindTexture(GL_TEXTURE_2D, this->texture[(int)TextureType::CLEAR]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data3 = stbi_load("clear.png", &width[(int)TextureType::CLEAR], &height[(int)TextureType::CLEAR], &nrChannels[(int)TextureType::CLEAR], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[(int)TextureType::CLEAR], height[(int)TextureType::CLEAR], 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data3);


	glGenTextures(1, &this->texture[(int)TextureType::INGAME]);
	glBindTexture(GL_TEXTURE_2D, this->texture[(int)TextureType::INGAME]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data4 = stbi_load("wallpaper.png", &width[(int)TextureType::INGAME], &height[(int)TextureType::INGAME], &nrChannels[(int)TextureType::INGAME], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[(int)TextureType::INGAME], height[(int)TextureType::INGAME], 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data4);
}

GLuint InGameManager::GetTexture(TextureType type) {
	return this->texture[(int)type];
}

void InGameManager::CreateGhost(int i, int j, Vector3 position) {
	Ghost* ghost = new Ghost(i, j, position, this->ghostID++);
	this->vGhost.push_back(ghost);
}

void InGameManager::DeleteGhost(Ghost* g) {
	vGhost.remove(g);
	delete g;
}

Ghost* InGameManager::FindGhostByID(int id) {
	//for (Ghost* g : vGhost) {
	//	if (g->GetID() == id)
	//		return g;
	//}

	//std::list<Ghost*>::iterator it;
	//it = vGhost.begin();
	//it = find(this->vGhost.begin(), this->vGhost.end(), 30);

	return NULL;
}

GLvoid InGameManager::DrawScene() {
	glUseProgram(s_program);
	std::list<Ghost*>::iterator it;
	this->CameraSetting(false);

	switch (this->state) {
	case GAMESTATE::LOBBY :
		this->startUI->DrawTextureImage(s_program);
		break;
	case GAMESTATE::INGAME:
	{
		//it = vGhost.begin();
		//while (it != vGhost.end())
		//{
		//	if ((*it)->GetIsActive()) {
		//		(*it)->DrawObject(s_program);
		//	}
		//	it++;
		//}
		this->map->DrawMap(s_program);

		PlayerInfo playerInfo = NetworkManager::GetInstance().GetPlayerInfo(player->id);// 인자로 넘겨준 ID의 플레이어 정보를 가져와서 세팅 (player->id는 내 아이디를 뜻함)
		//float dt = NetworkManager::GetInstance().GetDurationTime();

		Vector3 prevPlayerPos = player->GetPosition();
		Vector3 newPlayerPos(playerInfo.x, 0, playerInfo.z);
		Vector3 direction = newPlayerPos - prevPlayerPos;
		if (fabs(direction.x) < 0.01f) direction.x = 0.0f;
		if (fabs(direction.z) < 0.01f) direction.z = 0.0f;
		Vector3 currentPlayerPos = prevPlayerPos + direction * 6.0f * deltaTime * 0.001f;

		this->player->SetPosition(currentPlayerPos);
		this->player->DrawObject(s_program);
		// CheckDirection(this->player);
		for (int i = 0; i < 2; i++)
		{
			PlayerInfo playerInfo = NetworkManager::GetInstance().GetPlayerInfo(otherPlayer[i]->id);
			Vector3 pos(playerInfo.x, 0, playerInfo.z);
			otherPlayer[i]->SetPosition(pos);
			otherPlayer[i]->DrawObject(s_program);
		}
		this->bottom->DrawObject(s_program);
		break;
	}
	case GAMESTATE::GAMEOVER:
		this->endingUI->DrawTextureImage(s_program, TextureType::GAMEOVER);
		break;
	case GAMESTATE::CLEAR:
		this->endingUI->DrawTextureImage(s_program, TextureType::CLEAR);
		break;
	}
}

GLvoid InGameManager::DrawSubScene() {
	//this->endingUI->DrawTextureImage(s_program, TextureType::CLEAR);

	//std::list<Ghost*>::iterator it;
	switch (this->state) {
	case GAMESTATE::LOBBY:
		break;
	case GAMESTATE::INGAME:
		//it = vGhost.begin();
		//while (it != vGhost.end())
		//{
		//	if ((*it)->GetIsActive()) {
		//		(*it)->DrawObject(s_program);
		//	}
		//	it++;
		//}
		//this->player->DrawObject(s_program);
		//this->map->DrawMap(s_program);
		//this->bottom->DrawObject(s_program);
		this->ingameUI->PrintInGameUI(s_program);
		break;
	case GAMESTATE::GAMEOVER:
		break;
	case GAMESTATE::CLEAR:
		break;
	}
}

Vector3 InGameManager::DirToVec3(Direction dir) {
	switch (dir) {
	case Direction::LEFT:
		return Vector3(7.5, 0, 0);
	case Direction::RIGHT:
		return Vector3(-7.5, 0, 0);
	case Direction::UP:
		return Vector3(0, 0, 7.5);
	case Direction::DOWN:
		return Vector3(0, 0, -7.5);
	}
}

void InGameManager::SetNewPlayerDirection(int dir)
{
	if (dir == 1)
	{
		if (this->player->priorDirection == Direction::UP) {
			this->player->progressDirection = Direction::RIGHT;
		}
		else if (this->player->priorDirection == Direction::DOWN) {
			this->player->progressDirection = Direction::LEFT;
		}
		else if (this->player->priorDirection == Direction::RIGHT) {
			this->player->progressDirection = Direction::DOWN;
		}
		else if (this->player->priorDirection == Direction::LEFT) {
			this->player->progressDirection = Direction::UP;
		}
		this->player->isChangeCameraDir = true;
	}
	else if (dir == 0)
	{
		if (this->player->priorDirection == Direction::UP) {
			this->player->progressDirection = Direction::LEFT;
		}
		else if (this->player->priorDirection == Direction::DOWN) {
			this->player->progressDirection = Direction::RIGHT;
		}
		else if (this->player->priorDirection == Direction::RIGHT) {
			this->player->progressDirection = Direction::UP;
		}
		else if (this->player->priorDirection == Direction::LEFT) {
			this->player->progressDirection = Direction::DOWN;
		}
		this->player->isChangeCameraDir = true;
	}

}

float InGameManager::CountBeadAmount() {
	return this->beadNumber++;
}

void InGameManager::DecreaseBeadNumber() {
	this->beadNumber--;
}

float InGameManager::GetTime() {
	currentFrame = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	return lastFrame;
}

float InGameManager::GetPlayerHP() {
	if (this->DeleteHP) {
		this->GetPlayer()->hp -= 25;
		this->DeleteHP = false;
	}
	return this->GetPlayer()->hp;
}

void InGameManager::ChangeSpeed(float speed) {
	this->speed = speed;
}

void InGameManager::SetState(GAMESTATE state) {
	cout << (int)state << endl;
	this->state = state; 
	switch (state) {
	case GAMESTATE::CLEAR :
		RecordTime();
		break;
	case GAMESTATE::GAMEOVER :
		break;
	}
}

void InGameManager::InitGame() {
	delete this->player;
	delete this->map;
	delete this->bead;
	delete this->powerBead;
	delete this->ingameUI;
	delete this->bottom;
	this->beadNumber = 0;
	this->inGameTime = 0;
	this->player->hp = 100.0;

	this->InitObject();
}

void InGameManager::RecordTime() {
	ofstream writeFile;
	writeFile.open("test.txt", ios::app);    //파일 열기(파일이 없으면 만들어짐)

	string str = to_string(this->currentTime()) + " ";
	writeFile.write(str.c_str(), str.size());

	writeFile.close();    //꼭 닫아주기
}

string InGameManager::GetBestRecord() { //[TODO] 계속 불리지 않도록 수정
	ifstream readFile;
	readFile.open("test.txt");    //파일 열기
	float bestRecord = 0;
	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			string str;
			getline(readFile, str);
			if (str == "")
				break;
			float record = stof(str);
			if (bestRecord < record)
				bestRecord = record;

			//cout << str << endl;    //지금은 읽은 문자열 바로 출력.
		}
		readFile.close();    //파일 닫아줍니다.
	}
	return to_string(bestRecord);
}

//string InGameManager::GetNearByGhost() {
//	int count = 0;
//	for (Ghost* ghost : this->vGhost) {
//		int distance_i = abs(this->player->board_i - ghost->board_i);
//		int distance_j = abs(this->player->board_j - ghost->board_j);
//		if (distance_i < 4 && distance_j < 4)
//			count++;
//	}
//	return to_string(count);
//}


Vector3 Lerp(Vector3 value1, Vector3 value2, float amount)
{
	Vector3 newPos = Vector3(value1 + ((Vector3)(value2- value1) * amount));
	//cout << newPos.x << "," << newPos.y << "," << newPos.z << "," << endl;
	return newPos;
}

void InGameManager::CheckDirection(DynamicObject *dObject) {
	//float speed = (dObject->GetType() == ObjectType::DOOR) ? GHOST_SPEED : this->speed;
	// 회전 관련 코드
	if (dObject->isChangeCameraDir) {
		dObject->accDir += this->deltaTime * this->speed;
		dObject->dir = -Lerp(this->DirToVec3(dObject->priorDirection), this->DirToVec3(dObject->progressDirection), dObject->accDir).GetGlmVec3();
		//cout << dObject->dir.x << ", " << dObject->dir.y << ", " << dObject->dir.z << endl;
		// 회전 중!
		if (dObject->accDir > 1) { // 회전이 끝났을 때
			//cout << "fin" << endl;

			//cout << "Start : " << dObjectDirToVec3(this->player->priorDirection).x << ", " << this->DirToVec3(this->player->priorDirection).z << endl;
			//cout << "End : " << this->DirToVec3(this->player->progressDirection).x << ", " << this->DirToVec3(this->player->progressDirection).z << endl << endl;
			dObject->priorDirection = dObject->progressDirection;
			dObject->isChangeCameraDir = false;
			dObject->accDir = 0;
		}
		return; // 여기가 멈추는 로직
	}

	//// 플레이어가 이동 중이야, 근데 이동 중에 키를 눌렀어 다른 방향 키, 다른 방향 키 눌렀다고 해서 바로 반영되면 안되잖아. 왜냐면 도착해서 회전해야 i j 기반으로 움직이는거니까
	//// isArrived = true 인 순간이 i j 에 정확하게 도착한 순간인거지. 그래서 건들지 못하게 만드는거야 도착하기 전까지 새로운 키가 눌렸더라도.
	//if (dObject->isArrived && dObject->newDirection != Direction::NONE) {
	//	dObject->priorDirection = dObject->progressDirection; // priorDirection이 왜? 회전때문에 okay
	//	if (dObject->priorDirection == Direction::NONE) // 
	//		dObject->priorDirection = dObject->newDirection;
	//	dObject->progressDirection = dObject->newDirection;
	//	dObject->isArrived = false;
	//	dObject->acc = 0;

	//	if (dObject->progressDirection != dObject->priorDirection)
	//		dObject->isChangeCameraDir = true;
	//	// 이 순간이 회전하는 순간입니다.
	//	// 여기서 멈추고 회전을 다 하고 다음으로 이동합니다!
	//	// 멈추고 Lerp를 돌려야함.
	//}
	////cout <<"[" << dObject->board_i << "," << dObject->board_j <<  "]"<< dObject->GetPosition().x << ", " << dObject->GetPosition().z << endl;

	////cout << isArrived << endl;
	//StaticObject* target;
	//Vector3 startPos;
	//switch (dObject->progressDirection) {
	//case Direction::UP:
	//	if (dObject->board_i - 1 < 0) // 아에 빈 보드판으로 가지 않도록
	//	{
	//		break;
	//	}
	//	dObject->acc += this->deltaTime * speed;

	//	startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
	//	target = this->map->boardShape[dObject->board_i - 1][dObject->board_j];
	//	dObject->dir = this->DirToVec3(Direction::UP);

	//	if (dObject->isNewMapCollision) { // 다음 칸 도착했으면 그 다음 칸 찾기
	//		if (dObject->GetPosition() == target->GetPosition()) {
	//			//cout << "Collision!!" << endl;
	//			dObject->board_i = dObject->temp_i;
	//			dObject->board_j = dObject->temp_j;
	//			dObject->acc = 0;
	//			dObject->isArrived = true;
	//			startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
	//			target = this->map->boardShape[dObject->board_i - 1][dObject->board_j];
	//		}
	//	}
	//	else {
	//		dObject->isArrived = false;
	//	}

	//	if (target->GetType() == ObjectType::WALL) {
	//		if (dObject->GetType() == ObjectType::DOOR) {
	//			Ghost* temp = (Ghost*)dObject;
	//			temp->SetRandomDirection();
	//		}
	//		dObject->acc = 0;
	//		dObject->isArrived = true;
	//	}
	//	else {
	//		dObject->SetPlayerPos(Lerp(startPos, target->GetPosition(), dObject->acc));
	//	}
	//	break;

	//case Direction::DOWN:
	//	if (dObject->board_i + 1 > 30) // 아에 빈 보드판으로 가지 않도록
	//		break;
	//	dObject->acc += this->deltaTime * speed;

	//	startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
	//	target = this->map->boardShape[dObject->board_i + 1][dObject->board_j];
	//	//dir = target->GetPosition() - startPos;
	//	dObject->dir = this->DirToVec3(Direction::DOWN);
	//	//cout << "[" << dObject->board_i << "," << dObject->board_j << "]" << target->GetType() << endl;

	//	if (dObject->isNewMapCollision) { // 다음 칸 도착했으면 그 다음 칸 찾기
	//		if (dObject->GetPosition() == target->GetPosition()) {
	//			//cout << "Collision!!" << endl;
	//			dObject->board_i = dObject->temp_i;
	//			dObject->board_j = dObject->temp_j;
	//			dObject->acc = 0;
	//			dObject->isArrived = true;
	//			startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
	//			target = this->map->boardShape[dObject->board_i + 1][dObject->board_j];
	//		}
	//	}
	//	else {
	//		dObject->isArrived = false;
	//	}

	//	if (!(target->GetType() == ObjectType::WALL)) {
	//		dObject->SetPlayerPos(Lerp(startPos, target->GetPosition(), dObject->acc));
	//	}
	//	else {
	//		if (dObject->GetType() == ObjectType::DOOR) {
	//			Ghost* temp = (Ghost*)dObject;
	//			temp->SetRandomDirection();
	//		}
	//		dObject->acc = 0;
	//		dObject->isArrived = true;
	//	}
	//	break;

	//case Direction::LEFT:
	//	if (dObject->board_j - 1 < 0) // 아에 빈 보드판으로 가지 않도록
	//		break;
	//	dObject->acc += this->deltaTime * speed;

	//	startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
	//	target = this->map->boardShape[dObject->board_i][dObject->board_j - 1];
	//	//dir = target->GetPosition() - startPos;
	//	dObject->dir = this->DirToVec3(Direction::LEFT);
	//	//cout << "[" << dObject->board_i << "," << dObject->board_j << "]" << target->GetType() << endl;

	//	if (dObject->isNewMapCollision) { // 다음 칸 도착했으면 그 다음 칸 찾기
	//		if (dObject->GetPosition() == target->GetPosition()) {
	//			//cout << "Collision!!" << endl;
	//			dObject->board_i = dObject->temp_i;
	//			dObject->board_j = dObject->temp_j;
	//			dObject->acc = 0;
	//			dObject->isArrived = true;
	//			target = this->map->boardShape[dObject->board_i][dObject->board_j - 1];
	//			startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
	//		}
	//	}
	//	else {
	//		dObject->isArrived = false;
	//	}

	//	if (!(target->GetType() == ObjectType::WALL)) {
	//		dObject->SetPlayerPos(Lerp(startPos, target->GetPosition(), dObject->acc));
	//	}
	//	else {
	//		if (dObject->GetType() == ObjectType::DOOR) {
	//			Ghost* temp = (Ghost*)dObject;
	//			temp->SetRandomDirection();
	//		}
	//		dObject->acc = 0;
	//		dObject->isArrived = true;
	//	}
	//	break;

	//case Direction::RIGHT:
	//	if (player->board_j + 1 > 31) // 아에 빈 보드판으로 가지 않도록
	//		break;
	//	dObject->acc += this->deltaTime * speed;

	//	startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
	//	target = this->map->boardShape[dObject->board_i][dObject->board_j + 1];
	//	//cout << "[" << dObject->board_i << "," << dObject->board_j << "]" << target->GetType() << endl;
	//	//dir = target->GetPosition() - startPos;
	//	dObject->dir = this->DirToVec3(Direction::RIGHT);

	//	if (dObject->isNewMapCollision) { // 다음 칸 도착했으면 그 다음 칸 찾기
	//		if (dObject->GetPosition() == target->GetPosition()) {
	//			//cout << "Collision!!" << endl;
	//			dObject->board_i = dObject->temp_i;
	//			dObject->board_j = dObject->temp_j;
	//			dObject->acc = 0;
	//			dObject->isArrived = true;
	//			target = this->map->boardShape[dObject->board_i][dObject->board_j + 1];
	//			startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
	//		}
	//	}

	//	if (!(target->GetType() == ObjectType::WALL)) {
	//		dObject->SetPlayerPos(Lerp(startPos, target->GetPosition(), dObject->acc));
	//	}
	//	else {
	//		if (dObject->GetType() == ObjectType::DOOR) {
	//			Ghost* temp = (Ghost*)dObject;
	//			temp->SetRandomDirection();
	//		}
	//		dObject->acc = 0;
	//		dObject->isArrived = true;
	//	}
	//	break;
	//}
}

void InGameManager::PlayingBgm(const char* name) {
	return;
	mciSendCommand(this->dwID, MCI_CLOSE, 0, NULL); //dwID 음악 종료
	wchar_t text[30];
	size_t size;
	mbstowcs_s(&size, text, name, _TRUNCATE);
	openBgm.lpstrElementName = text; //파일 오픈
	openBgm.lpstrDeviceType = TEXT("waveaudio"); //wave 형식
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openBgm);
	this->dwID = openBgm.wDeviceID;
	mciSendCommand(this->dwID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)&playBgm); //음악 반복 재생
}

void InGameManager::PlayingFxSound(const char* name) {
	return;
	//mciSendCommand(this->dwID, MCI_CLOSE, 0, NULL); //dwID 음악 종료
	mciSendCommand(this->fxDwID, MCI_CLOSE, 0, NULL); //dwID 음악 종료

	wchar_t text[30];
	size_t size;
	mbstowcs_s(&size, text, name, _TRUNCATE);
	openFxSound.lpstrElementName = text; //파일 오픈
	openFxSound.lpstrDeviceType = TEXT("waveaudio"); //mp3 형식
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openFxSound);
	this->fxDwID = openFxSound.wDeviceID;
	mciSendCommand(this->fxDwID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&playFxSound); //음악을 한 번 재생
	//Sleep(1800); //효과음이 재생될 때까지 정지했다가
	//mciSendCommand(this->dwID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)NULL); //음원 재생 위치를 처음으로 초기화
}

default_random_engine dreColor_light((size_t)time(NULL));
normal_distribution <float>uidColor_light{ 0.0,1.0 };
int cycleCounter = 0;

void InGameManager::TimerFunction() {
	if (this->state != GAMESTATE::INGAME)
		return;

	this->GetTime();
	this->CalculateTime();
	this->CheckDirection(this->player);
	// 
}

GLvoid InGameManager::InitShader() {
	cout << "InitShader GameManager" << endl;
	make_vertexShaders();
	make_fragmentShaders();
	//-- shader Program
	s_program = glCreateProgram();
	glAttachShader(s_program, vertexShader);
	glAttachShader(s_program, fragmentShader);
	glLinkProgram(s_program);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
	glUseProgram(s_program);
}

// 여기는 오브젝트를 클래스로 찍어내는 곳, 인스턴스화
// + Obj 파일 정보 담아두는 코드
GLvoid InGameManager::InitObject() 
{
	this->SetState(GAMESTATE::NONE);

	this->startUI = new StartSceneUI();
	this->endingUI = new EndingScene();
	this->InitTexture();
	//this->block = new Block(Vector3(0, 0, 0));
	//this->block2 = new Block(Vector3(10.0, 0, 0));

	this->objData[(int)ObjectType::PLAYER] = new ObjData();
	this->objData[(int)ObjectType::WALL] = new ObjData();
	this->objData[(int)ObjectType::BEAD] = new ObjData();
	this->objData[(int)ObjectType::DOOR] = new ObjData();
	this->objData[(int)ObjectType::POWERBEAD] = new ObjData();
	this->objData[(int)ObjectType::BOTTOM] = new ObjData();

	this->player = new Player();
	for (int i = 0; i < MaxClients - 1; ++i)
	{
		this->otherPlayer[i] = new Player();
	}

	this->bead = new Bead();
	this->powerBead = new PowerBead();
	this->ingameUI = new InGameUI();
	this->bottom = new Bottom(Vector3(75,0,75));

	
	// this->player->SetPlayerPos(this->map->boardShape[this->player->board_i][this->player->board_i]->GetPosition().GetGlmVec3());
	this->SetCameraPos(this->player->GetPlayerPos().GetGlmVec3());
	this->CameraSetting(false);
	ReadObj(DOOR_FILE_NAME, this->objData[(int)ObjectType::DOOR]->vPosData, this->objData[(int)ObjectType::DOOR]->vNormalData, this->objData[(int)ObjectType::DOOR]->vTextureCoordinateData, this->objData[(int)ObjectType::DOOR]->indexData, this->objData[(int)ObjectType::DOOR]->vertexCount, this->objData[(int)ObjectType::DOOR]->indexCount);
	ReadObj(BEAD_FILE_NAME, this->objData[(int)ObjectType::BEAD]->vPosData, this->objData[(int)ObjectType::BEAD]->vNormalData, this->objData[(int)ObjectType::BEAD]->vTextureCoordinateData, this->objData[(int)ObjectType::BEAD]->indexData, this->objData[(int)ObjectType::BEAD]->vertexCount, this->objData[(int)ObjectType::BEAD]->indexCount);
	ReadObj(KEY_ITEM_FILE_NAME, this->objData[(int)ObjectType::POWERBEAD]->vPosData, this->objData[(int)ObjectType::POWERBEAD]->vNormalData, this->objData[(int)ObjectType::POWERBEAD]->vTextureCoordinateData, this->objData[(int)ObjectType::POWERBEAD]->indexData, this->objData[(int)ObjectType::POWERBEAD]->vertexCount, this->objData[(int)ObjectType::POWERBEAD]->indexCount);
	ReadObj(CUBE_FILE_NAME, this->objData[(int)ObjectType::WALL]->vPosData, this->objData[(int)ObjectType::WALL]->vNormalData, this->objData[(int)ObjectType::WALL]->vTextureCoordinateData, this->objData[(int)ObjectType::WALL]->indexData, this->objData[(int)ObjectType::WALL]->vertexCount, this->objData[(int)ObjectType::WALL]->indexCount);
	ReadObj(CUBE_FILE_NAME, this->objData[(int)ObjectType::PLAYER]->vPosData, this->objData[(int)ObjectType::PLAYER]->vNormalData, this->objData[(int)ObjectType::PLAYER]->vTextureCoordinateData, this->objData[(int)ObjectType::PLAYER]->indexData, this->objData[(int)ObjectType::PLAYER]->vertexCount, this->objData[(int)ObjectType::PLAYER]->indexCount);
	ReadObj(CUBE_FILE_NAME, this->objData[(int)ObjectType::BOTTOM]->vPosData, this->objData[(int)ObjectType::BOTTOM]->vNormalData, this->objData[(int)ObjectType::BOTTOM]->vTextureCoordinateData, this->objData[(int)ObjectType::BOTTOM]->indexData, this->objData[(int)ObjectType::BOTTOM]->vertexCount, this->objData[(int)ObjectType::BOTTOM]->indexCount);
	
	// vBlock.push_back(Block());
	this->isInitComplete = true;
}

void InGameManager::GameStart(start_game& startGame)
{
	// 플레이어 배열에 위치와 아이디, 타입까지 결정.
	// 타입이 0이면 고스트, 러너 플레이어로 그리기.

	this->player->id = NetworkManager::GetInstance().GetMyID();
	this->player->SetPlayerType(NetworkManager::GetInstance().GetPlayerInfo(this->player->id).type);
	int index = 0;
	for (int i = 0; i < 3; ++i) {
		if (i == this->player->id) continue;
		this->otherPlayer[index]->id = i;
		this->otherPlayer[index]->SetPlayerType(NetworkManager::GetInstance().GetPlayerInfo(this->otherPlayer[index]->id).type);
		index++;
	}
	this->map = new MapLoader(startGame.mapinfo);
	this->PlayingBgm(SOUND_FILE_NAME_INGAME);
	this->SetState(GAMESTATE::INGAME);
}

void InGameManager::RecvUpdateObject(object_status obj_info)
{
	Object obj = *this->map->boardShape[obj_info.row][obj_info.col];

	switch (obj.GetType()) {
	case ObjectType::WALL:
		break;
	case ObjectType::BEAD:
		if (this->player->playerType == PlayerType::RUNNER)
		{
			this->map->boardShape[obj_info.row][obj_info.col] = new StaticObject(this->map->boardShape[obj_info.row][obj_info.col]->GetPosition());
			// this->DecreaseBeadNumber();
			this->PlayingFxSound(SOUND_FILE_NAME_BEAD);
		}
		break;
	case ObjectType::POWERBEAD:
		if (this->player->playerType == PlayerType::RUNNER)
		{
			this->map->boardShape[obj_info.row][obj_info.col] = new StaticObject(this->map->boardShape[obj_info.row][obj_info.col]->GetPosition());
		}
		break;
	case ObjectType::ROAD:
		break;
	}
}

void InGameManager::RecvUpdateStatus(update_status update_status)
{
	if (this->state == GAMESTATE::INGAME)
	{
		for (int i = 0; i < 3; i++)
		{
			Player* targetPlayer = this->GetPlayer(i);
			if (this->GetPlayer(i)->playerType == PlayerType::RUNNER && update_status.player_active[i] == false)
			{
				if (targetPlayer->id == this->player->id) {
					this->SetState(GAMESTATE::GAMEOVER);
					return;
				}
				else
				{
					targetPlayer->isActive = false;
				}
			}
		}

		WinStatus winStatus = update_status.win;
		if (winStatus == WinStatus::NONE)
			return;

		PlayerInfo myPlayerInfo = NetworkManager::GetInstance().GetPlayerInfo(player->id);
		PlayerType playerType = myPlayerInfo.type;
		if (winStatus == WinStatus::RUNNER_WIN)
		{
			if (playerType == PlayerType::RUNNER)
				this->SetState(GAMESTATE::CLEAR);
			else
				this->SetState(GAMESTATE::GAMEOVER);
		}
		else if (winStatus == WinStatus::TAGGER_WIN)
		{
			if (playerType == PlayerType::TAGGER)
				this->SetState(GAMESTATE::CLEAR);
			else
				this->SetState(GAMESTATE::GAMEOVER);
		}
	}
}

Player* InGameManager::GetPlayer(int id)
{
	if (this->player->id == id)
		return this->player;
	else
	{
		for (int i = 0; i < MaxClients - 1; i++)
		{
			if (this->otherPlayer[i]->id == id)
				return this->otherPlayer[i];
		}
	}
}
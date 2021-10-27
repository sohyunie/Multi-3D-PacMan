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
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
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
		if (this->isFPS == true) {
			Vector3 dir = Vector3(this->player->GetPosition().x + this->player->dir.x, -1, this->player->GetPosition().z + this->player->dir.z);

			this->cameraDirection = dir.GetGlmVec3();
			this->cameraPos = this->player->GetPosition().GetGlmVec3();
			this->cameraPos.y += 1;
			//this->cameraPos.y += 1;
		}
		else {
			Vector3 dir = Vector3(this->player->GetPosition().x + this->player->dir.x, 0, this->player->GetPosition().z + this->player->dir.z);
			this->cameraDirection = dir.GetGlmVec3();
			this->cameraDirection.y += 5;
			this->cameraPos = this->player->GetPosition().GetGlmVec3();
			this->cameraPos.y += 100;
		}
	}
}

GLuint s_program;

GLvoid InGameManager::InitBuffer() {
	cout << "InitBuffer InGameManager" << endl;
	// PLAYER
	glGenVertexArrays(1, &this->VAO[PLAYER]);
	glBindVertexArray(this->VAO[PLAYER]);
	glGenBuffers(3, this->VBO[PLAYER]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[PLAYER][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[PLAYER]->vertexCount * sizeof(float) * 3, this->objData[PLAYER]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[PLAYER][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[PLAYER]->vertexCount * sizeof(float) * 3, this->objData[PLAYER]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[PLAYER][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[PLAYER]->vertexCount * sizeof(float) * 2, this->objData[PLAYER]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[PLAYER]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[PLAYER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[PLAYER]->indexCount * sizeof(int), this->objData[PLAYER]->indexData, GL_STATIC_DRAW);


	// GHOST
	glGenVertexArrays(1, &this->VAO[GHOST]);
	glBindVertexArray(this->VAO[GHOST]);
	glGenBuffers(3, this->VBO[GHOST]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[GHOST][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[GHOST]->vertexCount * sizeof(float) * 3, this->objData[GHOST]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[GHOST][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[GHOST]->vertexCount * sizeof(float) * 3, this->objData[GHOST]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[GHOST][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[GHOST]->vertexCount * sizeof(float) * 2, this->objData[GHOST]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[GHOST]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[GHOST]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[GHOST]->indexCount * sizeof(int), this->objData[GHOST]->indexData, GL_STATIC_DRAW);


	// BLOCK
	glGenVertexArrays(1, &this->VAO[WALL]);
	glBindVertexArray(this->VAO[WALL]);
	glGenBuffers(3, this->VBO[WALL]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[WALL][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[WALL]->vertexCount * sizeof(float) * 3, this->objData[WALL]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[WALL][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[WALL]->vertexCount * sizeof(float) * 3, this->objData[WALL]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[WALL][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[WALL]->vertexCount * sizeof(float) * 2, this->objData[WALL]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[WALL]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[WALL]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[WALL]->indexCount * sizeof(int), this->objData[WALL]->indexData, GL_STATIC_DRAW);


	// BEAD
	glGenVertexArrays(1, &this->VAO[BEAD]);
	glBindVertexArray(this->VAO[BEAD]);
	glGenBuffers(3, this->VBO[BEAD]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[BEAD][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[BEAD]->vertexCount * sizeof(float) * 3, this->objData[BEAD]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[BEAD][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[BEAD]->vertexCount * sizeof(float) * 3, this->objData[BEAD]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[BEAD][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[BEAD]->vertexCount * sizeof(float) * 2, this->objData[BEAD]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[BEAD]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[BEAD]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[BEAD]->indexCount * sizeof(int), this->objData[BEAD]->indexData, GL_STATIC_DRAW);



	// POWERBEAD
	glGenVertexArrays(1, &this->VAO[POWERBEAD]);
	glBindVertexArray(this->VAO[POWERBEAD]);
	glGenBuffers(3, this->VBO[POWERBEAD]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[POWERBEAD][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[POWERBEAD]->vertexCount * sizeof(float) * 3, this->objData[POWERBEAD]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[POWERBEAD][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[POWERBEAD]->vertexCount * sizeof(float) * 3, this->objData[POWERBEAD]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[POWERBEAD][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[POWERBEAD]->vertexCount * sizeof(float) * 2, this->objData[POWERBEAD]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[POWERBEAD]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[POWERBEAD]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[POWERBEAD]->indexCount * sizeof(int), this->objData[POWERBEAD]->indexData, GL_STATIC_DRAW);


	// BOTTOM
	glGenVertexArrays(1, &this->VAO[BOTTOM]);
	glBindVertexArray(this->VAO[BOTTOM]);
	glGenBuffers(3, this->VBO[BOTTOM]);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[BOTTOM][0]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[BOTTOM]->vertexCount * sizeof(float) * 3, this->objData[BOTTOM]->vPosData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[BOTTOM][1]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[BOTTOM]->vertexCount * sizeof(float) * 3, this->objData[BOTTOM]->vNormalData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[BOTTOM][2]);
	glBufferData(GL_ARRAY_BUFFER, this->objData[BOTTOM]->vertexCount * sizeof(float) * 2, this->objData[BOTTOM]->vTextureCoordinateData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->EBO[BOTTOM]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[BOTTOM]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->objData[BOTTOM]->indexCount * sizeof(int), this->objData[BOTTOM]->indexData, GL_STATIC_DRAW);


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
	glGenVertexArrays(1, &this->VAO[TEXTURE]);
	glBindVertexArray(VAO[TEXTURE]);
	glGenBuffers(3, this->VBO[TEXTURE]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[TEXTURE][0]);
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
	this->CameraSetting(this->isFPS);

	//this->state= GAMESTATE::INGAME;
	switch (this->state) {
	case GAMESTATE::LOBBY :
		this->startUI->DrawTextureImage(s_program);
		break;
	case GAMESTATE::INGAME:
		it = vGhost.begin();
		while (it != vGhost.end())
		{
			if ((*it)->GetIsActive()) {
				(*it)->DrawObject(s_program);
			}
			it++;
		}
		this->player->DrawObject(s_program);
		this->map->DrawMap(s_program);
		this->bottom->DrawObject(s_program);
		break;
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

Vector3 InGameManager::DirToVec3(DIRECTION dir) {
	switch (dir) {
	case DIRECTION::LEFT:
		return Vector3(0, 0, -7.5);
	case DIRECTION::RIGHT:
		return Vector3(0, 0, 7.5);
	case DIRECTION::UP:
		return Vector3(-7.5, 0, 0);
	case DIRECTION::DOWN:
		return Vector3(7.5, 0, 0);
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
		this->GetPlayer()->hp -= 5;
		this->DeleteHP = false;
	}
	return this->GetPlayer()->hp;
}

void InGameManager::ChangeSpeed(float speed) {
	this->speed = speed;
}

void InGameManager::SetState(GAMESTATE state) {
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

string InGameManager::GetNearByGhost() {
	int count = 0;
	for (Ghost* ghost : this->vGhost) {
		int distance_i = abs(this->player->board_i - ghost->board_i);
		int distance_j = abs(this->player->board_j - ghost->board_j);
		if (distance_i < 4 && distance_j < 4)
			count++;
	}
	return to_string(count);
}


Vector3 Lerp(Vector3 value1, Vector3 value2, float amount)
{
	Vector3 newPos = Vector3(value1 + ((Vector3)(value2 - value1) * amount));
	//cout << newPos.x << "," << newPos.y << "," << newPos.z << "," << endl;
	return newPos;
}

void InGameManager::CheckDirection(DynamicObject *dObject) {
	float speed = (dObject->GetType() == ObjectType::GHOST) ? GHOST_SPEED : this->speed;

	// 회전 관련 코드
	if (dObject->isChangeCameraDir) {
		dObject->accDir += this->deltaTime * speed;
		dObject->dir = Lerp(this->DirToVec3(dObject->priorDirection), this->DirToVec3(dObject->progressDirection), dObject->accDir).GetGlmVec3();
		// 회전 중!
		if (dObject->accDir > 1) { // 회전이 끝났을 때
			//cout << "fin" << endl;

			//cout << "Start : " << dObjectDirToVec3(this->player->priorDirection).x << ", " << this->DirToVec3(this->player->priorDirection).z << endl;
			//cout << "End : " << this->DirToVec3(this->player->progressDirection).x << ", " << this->DirToVec3(this->player->progressDirection).z << endl << endl;
			dObject->isChangeCameraDir = false;
			dObject->accDir = 0;
		}
		return;
	}

	// 플레이어가 이동 중이야, 근데 이동 중에 키를 눌렀어 다른 방향 키, 다른 방향 키 눌렀다고 해서 바로 반영되면 안되잖아. 왜냐면 도착해서 회전해야 i j 기반으로 움직이는거니까
	// isArrived = true 인 순간이 i j 에 정확하게 도착한 순간인거지. 그래서 건들지 못하게 만드는거야 도착하기 전까지 새로운 키가 눌렸더라도.
	if (dObject->isArrived && dObject->newDirection != DIRECTION::DIR_NONE) {
		dObject->priorDirection = dObject->progressDirection; // priorDirection이 왜? 회전때문에 okay
		if (dObject->priorDirection == DIRECTION::DIR_NONE) // 
			dObject->priorDirection = dObject->newDirection;
		dObject->progressDirection = dObject->newDirection;
		dObject->isArrived = false;
		dObject->acc = 0;

		if (dObject->progressDirection != dObject->priorDirection)
			dObject->isChangeCameraDir = true;
		// 이 순간이 회전하는 순간입니다.
		// 우리가 해줘야 할 것은?!
		// 여기서 멈추고 회전을 다 하고 다음으로 이동합니다!
		// 멈추고 Lerp를 돌려야함.
	}
	//cout <<"[" << dObject->board_i << "," << dObject->board_j <<  "]"<< dObject->GetPosition().x << ", " << dObject->GetPosition().z << endl;

	//cout << isArrived << endl;
	StaticObject* target;
	Vector3 startPos;
	switch (dObject->progressDirection) {
	case DIRECTION::UP:
		if (dObject->board_i - 1 < 0) // 아에 빈 보드판으로 가지 않도록
		{
			break;
		}
		dObject->acc += this->deltaTime * speed;

		startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
		target = this->map->boardShape[dObject->board_i - 1][dObject->board_j];
		dObject->dir = this->DirToVec3(DIRECTION::UP);

		if (dObject->isNewMapCollision) { // 다음 칸 도착했으면 그 다음 칸 찾기
			if (dObject->GetPosition() == target->GetPosition()) {
				//cout << "Collision!!" << endl;
				dObject->board_i = dObject->temp_i;
				dObject->board_j = dObject->temp_j;
				dObject->acc = 0;
				dObject->isArrived = true;
				startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
				target = this->map->boardShape[dObject->board_i - 1][dObject->board_j];
			}
		}
		else {
			dObject->isArrived = false;
		}

		if (target->GetType() == ObjectType::WALL) {
			if (dObject->GetType() == ObjectType::GHOST) {
				Ghost* temp = (Ghost*)dObject;
				temp->SetRandomDirection();
			}
			dObject->acc = 0;
			dObject->isArrived = true;
		}
		else {
			dObject->SetPlayerPos(Lerp(startPos, target->GetPosition(), dObject->acc));
		}
		break;

	case DIRECTION::DOWN:
		if (dObject->board_i + 1 > 30) // 아에 빈 보드판으로 가지 않도록
			break;
		dObject->acc += this->deltaTime * speed;

		startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
		target = this->map->boardShape[dObject->board_i + 1][dObject->board_j];
		//dir = target->GetPosition() - startPos;
		dObject->dir = this->DirToVec3(DIRECTION::DOWN);
		//cout << "[" << dObject->board_i << "," << dObject->board_j << "]" << target->GetType() << endl;

		if (dObject->isNewMapCollision) { // 다음 칸 도착했으면 그 다음 칸 찾기
			if (dObject->GetPosition() == target->GetPosition()) {
				//cout << "Collision!!" << endl;
				dObject->board_i = dObject->temp_i;
				dObject->board_j = dObject->temp_j;
				dObject->acc = 0;
				dObject->isArrived = true;
				startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
				target = this->map->boardShape[dObject->board_i + 1][dObject->board_j];
			}
		}
		else {
			dObject->isArrived = false;
		}

		if (!(target->GetType() == ObjectType::WALL)) {
			dObject->SetPlayerPos(Lerp(startPos, target->GetPosition(), dObject->acc));
		}
		else {
			if (dObject->GetType() == ObjectType::GHOST) {
				Ghost* temp = (Ghost*)dObject;
				temp->SetRandomDirection();
			}
			dObject->acc = 0;
			dObject->isArrived = true;
		}
		break;

	case DIRECTION::LEFT:
		if (dObject->board_j - 1 < 0) // 아에 빈 보드판으로 가지 않도록
			break;
		dObject->acc += this->deltaTime * speed;

		startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
		target = this->map->boardShape[dObject->board_i][dObject->board_j - 1];
		//dir = target->GetPosition() - startPos;
		dObject->dir = this->DirToVec3(DIRECTION::LEFT);
		//cout << "[" << dObject->board_i << "," << dObject->board_j << "]" << target->GetType() << endl;

		if (dObject->isNewMapCollision) { // 다음 칸 도착했으면 그 다음 칸 찾기
			if (dObject->GetPosition() == target->GetPosition()) {
				//cout << "Collision!!" << endl;
				dObject->board_i = dObject->temp_i;
				dObject->board_j = dObject->temp_j;
				dObject->acc = 0;
				dObject->isArrived = true;
				target = this->map->boardShape[dObject->board_i][dObject->board_j - 1];
				startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
			}
		}
		else {
			dObject->isArrived = false;
		}

		if (!(target->GetType() == ObjectType::WALL)) {
			dObject->SetPlayerPos(Lerp(startPos, target->GetPosition(), dObject->acc));
		}
		else {
			if (dObject->GetType() == ObjectType::GHOST) {
				Ghost* temp = (Ghost*)dObject;
				temp->SetRandomDirection();
			}
			dObject->acc = 0;
			dObject->isArrived = true;
		}
		break;

	case DIRECTION::RIGHT:
		if (player->board_j + 1 > 31) // 아에 빈 보드판으로 가지 않도록
			break;
		dObject->acc += this->deltaTime * speed;

		startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
		target = this->map->boardShape[dObject->board_i][dObject->board_j + 1];
		//cout << "[" << dObject->board_i << "," << dObject->board_j << "]" << target->GetType() << endl;
		//dir = target->GetPosition() - startPos;
		dObject->dir = this->DirToVec3(DIRECTION::RIGHT);

		if (dObject->isNewMapCollision) { // 다음 칸 도착했으면 그 다음 칸 찾기
			if (dObject->GetPosition() == target->GetPosition()) {
				//cout << "Collision!!" << endl;
				dObject->board_i = dObject->temp_i;
				dObject->board_j = dObject->temp_j;
				dObject->acc = 0;
				dObject->isArrived = true;
				target = this->map->boardShape[dObject->board_i][dObject->board_j + 1];
				startPos = this->map->boardShape[dObject->board_i][dObject->board_j]->GetPosition();
			}
		}

		//cout << target->GetType() << endl;
		if (!(target->GetType() == ObjectType::WALL)) {
			dObject->SetPlayerPos(Lerp(startPos, target->GetPosition(), dObject->acc));
		}
		else {
			if (dObject->GetType() == ObjectType::GHOST) {
				Ghost* temp = (Ghost*)dObject;
				temp->SetRandomDirection();
			}
			dObject->acc = 0;
			dObject->isArrived = true;
		}
		break;
	}
}

void InGameManager::PlayingBgm(const char* name) {
	mciSendCommand(this->dwID, MCI_CLOSE, 0, NULL); //dwID 음악 종료
	wchar_t text[30];

	mbstowcs(text, name, strlen(name) + 1);
	openBgm.lpstrElementName = text; //파일 오픈
	openBgm.lpstrDeviceType = TEXT("waveaudio"); //wave 형식
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openBgm);
	this->dwID = openBgm.wDeviceID;
	mciSendCommand(this->dwID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)&playBgm); //음악 반복 재생
}

void InGameManager::PlayingFxSound(const char* name) {
	//mciSendCommand(this->dwID, MCI_CLOSE, 0, NULL); //dwID 음악 종료
	mciSendCommand(this->fxDwID, MCI_CLOSE, 0, NULL); //dwID 음악 종료

	wchar_t text[30];
	mbstowcs(text, name, strlen(name) + 1);
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
	//ghostSpawnRunningTime += this->GetDeltaTime();
	//if (ghostSpawnRunningTime > GHOST_SPAWN_TIME) {
	//	ghostSpawnRunningTime = 0;
	//	for (Ghost* g : this->vGhost) {
	//		if (!g->GetIsActive()) {
	//			g->SetIsActive(true);
	//			break;
	//		}
	//	}
	//}
	if (this->EatPowerBead == true) {
		this->powerBeadTime += this->deltaTime;
		cycleCounter++;
		if (cycleCounter % 5 == 0) {
			if (lightColor == lightColor_white) {
				lightColor = lightColor_black;
			}
			else {
				lightColor = lightColor_white;
			}
		}
		/*lightColor.x = uidColor_light(dreColor_light);
		lightColor.y = uidColor_light(dreColor_light);
		lightColor.z = uidColor_light(dreColor_light);*/
		if (this->powerBeadTime > POWER_BEAD_TIME) {
			cout << this->powerBeadTime << endl;
			lightColor = lightColor_white;
			this->EatPowerBead = false;
			this->ChangeSpeed(NORMAL_SPEED);
		}
	}

	for (int i = 0; i < MAP_SIZE; ++i) {
		for (int j = 0; j < MAP_SIZE; ++j) {
			Object obj = *this->map->boardShape[i][j];

			// Player
			bool isCollision = this->player->CollisionCheck(obj);
			if (isCollision) {
				//cout << "isCollision!! : " << i << "," << j << " : " << obj.GetType() << endl;
				switch (obj.GetType()) {
				case ObjectType::WALL:
					break;
				case ObjectType::BEAD:
					// isMapCollision : <딱 부딪친 순간만> 체크하는 bool이에요!
					// player boar i j  // 새롭게 부딪친 i j
					this->player->isNewMapCollision = (!(this->player->board_i == i && this->player->board_j == j)); // 출발점이 
					//cout << "BEAD" << endl;
					this->player->temp_i = i;
					this->player->temp_j = j;
					this->map->boardShape[i][j] = new StaticObject(this->map->boardShape[i][j]->GetPosition());
					this->DecreaseBeadNumber();
					this->PlayingFxSound(SOUND_FILE_NAME_BEAD);
					//PlaySound(TEXT(SOUND_FILE_NAME_BEAD), NULL, SND_ASYNC | SND_SYNC); // 반응 느린건 일단 이따 생각할게유!  우웅ㅇ
					cout << "멍멍" << endl;
					if(this->GetBeadCount() <= 0)
						this->SetState(GAMESTATE::CLEAR);
					// cout << "beadNumber: " << this->beadNumber << endl;
					break;
				case ObjectType::POWERBEAD:
					this->player->isNewMapCollision = (!(this->player->board_i == i && this->player->board_j == j));
					this->player->temp_i = i;
					this->player->temp_j = j;
					this->map->boardShape[i][j] = new StaticObject(this->map->boardShape[i][j]->GetPosition());
					this->EatPowerBead = true;
					this->ChangeSpeed(POWER_SPEED);
					// n초 동안 무적 상태
					break;
				case ObjectType::ROAD:
					this->player->isNewMapCollision = (!(this->player->board_i == i && this->player->board_j == j));
					//cout << "ROAD" << endl;
					this->player->temp_i = i;
					this->player->temp_j = j;
					break;
				}
			}

			// Ghost
			for (Ghost* g : this->vGhost) {
				bool isCollision = g->CollisionCheck(obj);
				if (isCollision) {
					//cout << "isCollision!! : " << i << "," << j << " : " << obj.GetType() << endl;
					switch (obj.GetType()) {
					case ObjectType::WALL:
						break;
					case ObjectType::BEAD:
						g->isNewMapCollision = (!(g->board_i == i && g->board_j == j));
						g->temp_i = i;
						g->temp_j = j;
						break;
					case ObjectType::POWERBEAD:
						g->isNewMapCollision = (!(g->board_i == i && g->board_j == j));
						g->temp_i = i;
						g->temp_j = j;
						break;
					case ObjectType::ROAD:
						g->isNewMapCollision = (!(g->board_i == i && g->board_j == j));
						//cout << "ROAD" << endl;
						g->temp_i = i;
						g->temp_j = j;
						break;
					}
				}
			}
		}
	}

	// Player to Ghost
	std::list<Ghost*>::iterator it;
	it = vGhost.begin();
	Ghost* ghost = nullptr;
	while (it != vGhost.end())
	{
		bool isCollision = (*it)->CollisionCheck(*this->player);	// (*it)로 쓰면, n번째 값에 접근하는 형태 | 엄청중요! STL은 이게 다다!!
		if (isCollision) {
			if (this->EatPowerBead) {
				ghost = (*it);
			}
			else {
				bool isCollisionGhost = false;

				// 충돌했던 고스트인지 확인, 이미 충돌되어 있으면 isCollisionGhost를 true로 만들어서 2초동안 체력 안깎이게
				for (GhostCollisionData* ghost : this->collisionGhost) {
					if ((*it)->GetID() == ghost->ghost->GetID()) {
						isCollisionGhost = true;
						break;
					}
				}

				if (!isCollisionGhost) {
					this->GetPlayer()->hp -= 5;
					this->collisionGhost.push_back(new GhostCollisionData((*it), COLLISION_TIME));
					cout << this->GetPlayer()->hp << endl;
					if (this->GetPlayer()->hp <= 0)
						this->SetState(GAMESTATE::GAMEOVER);
				}

			}
		}
		it++;
	}

	GhostCollisionData* TimeOutGhost = nullptr;	// 충돌하고 2초 지난 ghost
	for (GhostCollisionData *ghost : this->collisionGhost) {
		ghost->time -= this->GetDeltaTime();
		if (ghost->time < 0) {
			TimeOutGhost = ghost;
			break;
		}
	}

	// 무적 상태일 때 ghost 지우기
	if (ghost != nullptr)
		this->DeleteGhost(ghost);

	// 시간 끝난 ghost 지우기 ; 게임 내에서 삭제하는 게 아니라 2초 다시 셋팅해준다고 생각하면 됨
	if (TimeOutGhost != nullptr)
		this->collisionGhost.remove(TimeOutGhost);

	this->GetTime();
	this->CalculateTime();
	for (Ghost* g : this->vGhost) {
		this->CheckDirection(g);
	}
	this->CheckDirection(this->player);
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
	this->SetState(GAMESTATE::LOBBY);

	this->startUI = new StartSceneUI();
	this->endingUI = new EndingScene();
	this->InitTexture();
	//this->block = new Block(Vector3(0, 0, 0));
	//this->block2 = new Block(Vector3(10.0, 0, 0));

	this->objData[PLAYER] = new ObjData();
	this->objData[WALL] = new ObjData();
	this->objData[BEAD] = new ObjData();
	this->objData[GHOST] = new ObjData();
	this->objData[POWERBEAD] = new ObjData();
	this->objData[BOTTOM] = new ObjData();

	this->player = new Player();
	this->map = new MapLoader(0);
	this->bead = new Bead();
	this->powerBead = new PowerBead();
	this->ingameUI = new InGameUI();
	this->bottom = new Bottom(Vector3(75,0,75));
	//this->player->SetPlayerPos(this->map->boardShape[this->player->board_i][this->player->board_i]->GetPosition().GetGlmVec3());
	this->SetCameraPos(this->player->GetPlayerPos().GetGlmVec3());
	this->CameraSetting(this->isFPS);
	ReadObj(FILE_NAME, this->objData[GHOST]->vPosData, this->objData[GHOST]->vNormalData, this->objData[GHOST]->vTextureCoordinateData, this->objData[GHOST]->indexData, this->objData[GHOST]->vertexCount, this->objData[GHOST]->indexCount);
	ReadObj(BEAD_FILE_NAME, this->objData[BEAD]->vPosData, this->objData[BEAD]->vNormalData, this->objData[BEAD]->vTextureCoordinateData, this->objData[BEAD]->indexData, this->objData[BEAD]->vertexCount, this->objData[BEAD]->indexCount);
	ReadObj(BEAD_FILE_NAME, this->objData[POWERBEAD]->vPosData, this->objData[POWERBEAD]->vNormalData, this->objData[POWERBEAD]->vTextureCoordinateData, this->objData[POWERBEAD]->indexData, this->objData[POWERBEAD]->vertexCount, this->objData[POWERBEAD]->indexCount);
	ReadObj(CUBE_FILE_NAME, this->objData[WALL]->vPosData, this->objData[WALL]->vNormalData, this->objData[WALL]->vTextureCoordinateData, this->objData[WALL]->indexData, this->objData[WALL]->vertexCount, this->objData[WALL]->indexCount);
	ReadObj(CUBE_FILE_NAME, this->objData[PLAYER]->vPosData, this->objData[PLAYER]->vNormalData, this->objData[PLAYER]->vTextureCoordinateData, this->objData[PLAYER]->indexData, this->objData[PLAYER]->vertexCount, this->objData[PLAYER]->indexCount);
	ReadObj(CUBE_FILE_NAME, this->objData[BOTTOM]->vPosData, this->objData[BOTTOM]->vNormalData, this->objData[BOTTOM]->vTextureCoordinateData, this->objData[BOTTOM]->indexData, this->objData[BOTTOM]->vertexCount, this->objData[BOTTOM]->indexCount);
	cout << "test" << endl;
	// vBlock.push_back(Block());
	this->isInitComplete = true;
}
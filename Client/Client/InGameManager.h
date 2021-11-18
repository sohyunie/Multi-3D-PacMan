#pragma once
class Block;    // 기능은 못쓰고 Block이라는 애가 존재해만 알 수 있음
class Ghost;
class Player;
class MapLoader;
class Bead;
class PowerBead;
class Object;
class InGameUI;
class DynamicObject;
class Bottom;
class StartSceneUI;
class EndingScene;

#include <algorithm>
#include <list>

class InGameManager
{
private:
	static InGameManager* instance; // *쓰는 이유: 하나만 존재하게 하려고 / * 이거 안쓰면 메모리가 잡혀버리고 시작. 동적할당은 그 때 그때 메모리를 새로 할당/ 메모리 낭비 방지하려는 차원/
    // static으로 메모리 잡히면? 프로그램 컴파일 했을 때 즉시로 메모리가 잡힘 / 그냥 변수 쓰면 저 변수를 호출할 때 잡힘
    // static을 사용하는 이유: 프로그램 내에서 메모리를 옮길 필요없는 변수라서, 프로그램 내에서 메모리를 한번만 차지하면 되서?
    GLuint VAO[MAX_VAO_TYPE];
    GLuint VBO[MAX_VAO_TYPE][3];
    GLuint EBO[MAX_VAO_TYPE];
    Block* block;
    Block* block2;
    //Ghost* ghost[20];
    list<Ghost*> vGhost;
    list<GhostCollisionData*> collisionGhost;
    Object* object;
    ObjData* objData[MAX_VAO_TYPE];
    Player* player;
    MapLoader* map;
    Bead* bead;
    PowerBead* powerBead;
    InGameUI* ingameUI;
    Bottom* bottom;
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.25f); 
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 offset = glm::vec3(0.0f, 15.5f, 0.0f); // 카메라 오차범위계산 눈은 위에 달림

    float speed = NORMAL_SPEED;
    float degreeCameraRotate = 45.0f; // 요고야 요고 근데 얘 안쓰고 있을걸?  
    float degreeLightPos = 0.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame = 0.0f;
    float additonalTime = 0.0f;
    float inGameTime = 0.0f;
    float powerBeadTime = 0.0f;
    float angle, lx, lz = 0.0f;
    float ghostSpawnRunningTime = 0.0f;
    glm::vec3 ghostColor = glm::vec3(0.0f, 0.0f, 0.0f);
    int beadNumber = 0;
    bool isDrawFill = true;
    bool isFPS = true;
    bool isGhost = false;
    bool isBead = true;
    bool EatBead = false;
    bool EatPowerBead = false;
    bool CollideBead = false;
    bool isPowerBead = false;
    bool isInitComplete = false;
    bool DeleteHP = false;
    int ghostID;
    int key;
    Vector3 lightColor = Vector3(1, 1, 1);
    Vector3 lightColor_white = Vector3(1, 1, 1);
    Vector3 lightColor_black = Vector3(0, 0, 0);
    GAMESTATE state = GAMESTATE::NONE;
    GLuint texture[4];
    StartSceneUI* startUI;
    EndingScene* endingUI;
    unsigned int flaglocation;
    int dwID; // channel
    int fxDwID;
public:
    static InGameManager& GetInstance() {
        if (instance == NULL) {
            instance = new InGameManager();
        }
        return *instance;
    }

	GLvoid DrawScene();
    GLvoid DrawSubScene();
    GLvoid InitBuffer();
    GLvoid InitShader();
    GLvoid InitObject();
    GLvoid InitScene();

    void GameStart(start_game recvStartGame);

    float GetDegreeCameraRotate() { return this->degreeCameraRotate; }
    float GetDegreeLightPos() { return this->degreeLightPos; }
    float GetDeltaTime() { return this->deltaTime; }
    float GetIngameTime() { return this->inGameTime; }
    float currentTime() { return round(this->inGameTime / 100) / 10 + this->additonalTime; }
    float GetTime();
    void RecordTime();
    string GetBestRecord();
    float GetPlayerHP();
    //int GetBeadNumber() { return this->beadNumber; }
    bool GetPresence() { return this->isBead; }
    void CalculateTime();
    void CameraSetting(bool isFps);
    void TimerFunction();
    void CheckDirection(DynamicObject* dObject);
    Vector3 DirToVec3(Direction dir);
    void CreateGhost(int i, int j, Vector3 position);
    void DeleteGhost(Ghost* g);
    Ghost* FindGhostByID(int id);
    float CountBeadAmount();
    void DecreaseBeadNumber();
    string GetNearByGhost();

    void DrawTextureImage();
    GLuint GetTexture(TextureType type);

    bool GetIsDrawFill() { return this->isDrawFill; }
    glm::vec3 GetCameraPos() { return this->cameraPos; }
    glm::vec3 GetCameraDirection() { return this->cameraDirection; }
    glm::vec3 GetCameraUp() { return this->cameraUp; }
    void SetCameraPos(glm::vec3 camera) { this->cameraPos = camera + offset; }; // set은 void / return타입이 없어도됨
    void SetDegreeCameraRotate(float rotate) { this->degreeCameraRotate = rotate; }
    void SetCameraDirection(glm::vec3 dir) {   this->cameraDirection = dir; }
    void SetFPS(bool isOn) { this->isFPS = isOn; }
    Vector3 GetLightColor() { return this->lightColor; }
    void SetLightColor(Vector3 color) { this->lightColor = color; }
    Ghost* GetGhost();
    // float GetHP();

    Player* GetPlayer() { return this->player; }    // GM에서 player를 불러서 사용하고 싶으니까 여기서 getplayer를 만들어서 한 싱글턴 구조 안에서 player불러서 사용할 수 있게 함
    MapLoader* LoadMap() { return this->map; }
    InGameUI* GetInGameUI() { return this->ingameUI; }
    GLvoid DrawMap(){}
    bool GetFPS() { return this->isFPS; }
    void ChangeSpeed(float speed);
    void InitGame();

    GLint GetVAO(ObjectType type) { return this->VAO[(int)type]; }
    ObjData* GetObjData(ObjectType type) { return this->objData[(int)type]; }
    GAMESTATE GetState() { return this->state; }
    int GetBeadCount() { return this->beadNumber; }
    void SetState(GAMESTATE state);
    void InitTexture();

    void PlayingBgm(const char* name);
    void PlayingFxSound(const char* name);
protected:

};
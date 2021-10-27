#include "Standard.h"
#include "Ghost.h"
#include "InGameManager.h"

default_random_engine dreColor((size_t)time(NULL));
normal_distribution <float>uidColor{ 0.0,1.0 };
uniform_int_distribution <int>uidDirection{ 0,3 };

// 생성자
// 헤더랑 따로 한 이유: 헤더에서는 이 클래스가 어떤 역할 하는지 보기 쉽게 만드는 거고 잡다한게 써있으면 복잡하니까 정의들은 cpp에다가 풀어놓는다.
Ghost::Ghost() {
	this->type = ObjectType::GHOST;
}

Ghost::Ghost(int i, int j, Vector3 pos, int id) {
	this->id;
	this->type = ObjectType::GHOST;
	this->position = pos;
	this->board_i = i;
	this->board_j = j;
	this->scale = Vector3(0.5,0.5,0.5);
	this->rotate = Vector3(0.0, 1.0, 0.0);
	this->color = glm::vec3(uidColor(dreColor), uidColor(dreColor), uidColor(dreColor));
	this->newDirection = (DIRECTION)uidDirection(dreColor);
	this->boundingOffset = 1.5;
}

void Ghost::SetRandomDirection() {
	this->newDirection = (DIRECTION)uidDirection(dreColor);
}

void Ghost::DrawObject(GLuint s_program) {
	if (this->isActive == false)
		return;
	glm::mat4 STR = glm::mat4(1.0f); //--- transformation matrix
	glm::mat4 R = glm::mat4(1.0f); //--- rotation matrix
	glm::mat4 T = glm::mat4(1.0f); //--- transformation matrix
	glm::mat4 S = glm::mat4(1.0f); //--- rotation matrix
	T = glm::translate(T, this->position.GetGlmVec3(-2)); //--- x축으로 translation
	R = glm::rotate(R, glm::radians(rotateAngle+=10), this->rotate.GetGlmVec3()); //--- z축에대하여 회전
	S = glm::scale(glm::mat4(1.0f), this->scale.GetGlmVec3());
	STR = T * S * R; //--- 합성 변환 행렬: translate -> rotate

	Vector3 lightColor = InGameManager::GetInstance().GetLightColor();
	glm::vec3 cameraPos = InGameManager::GetInstance().GetCameraPos();
	glm::vec3 cameraDirection = InGameManager::GetInstance().GetCameraDirection();
	glm::vec3 cameraUp = InGameManager::GetInstance().GetCameraUp();

	glm::mat4 view = glm::lookAt(glm::vec3(cameraPos), cameraDirection, cameraUp);
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), WINDOW_WITDH / (float)WINDOW_HEIGHT, 0.001f, 1000.f);

	unsigned int modelLocation = glGetUniformLocation(s_program, "g_modelTransform"); //--- 버텍스 세이더에서모델 변환 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(STR)); //--- modelTransform 변수에 변환 값 적용하기

	unsigned int viewLocation = glGetUniformLocation(s_program, "g_view");	// 버텍스 사이에서 viewTransform 변수위치
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));	// viewTransform 변수에 변환값 적용하기

	unsigned int projectionLocation = glGetUniformLocation(s_program, "g_projection");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(proj));

	int lightPosLocation = glGetUniformLocation(s_program, "g_lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, lightPos.x, lightPos.y, lightPos.z);

	int lightColorLocation = glGetUniformLocation(s_program, "g_lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, lightColor.x, lightColor.y, lightColor.z);

	int objColorLocation = glGetUniformLocation(s_program, "g_objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(objColorLocation, this->color.x, this->color.y, this->color.z);

	int ViewLocation = glGetUniformLocation(s_program, "g_cameraPos");
	glUniform3f(ViewLocation, cameraPos.x, cameraPos.y, cameraPos.z);

	unsigned int flaglocation = glGetUniformLocation(s_program, "flag");
	glUniform1i(flaglocation, 0);

	// 사용할 VAO 불러오기
	glBindVertexArray(InGameManager::GetInstance().GetVAO(this->type));
	// 삼각형 그리기
	glDrawElements(GL_TRIANGLES, InGameManager::GetInstance().GetObjData(this->type)->indexCount, GL_UNSIGNED_INT, 0);
}
#include "Standard.h"
#include "Door.h"
#include "InGameManager.h"

//default_random_engine dreColor_block((size_t)time(NULL));
//normal_distribution <float>uidColor_block{ 0.0,1.0 };

Door::Door() {
	this->type = ObjectType::DOOR;
}

Door::Door(Vector3 pos) {
	this->type = ObjectType::DOOR;
	this->position = pos;	// position�� �׳� �����ڿ��� ���ڷ� �ȹ޾ƿ��� ���⼭ �����ϸ� �ȵǳ�..? ingame���� ���� ������ �־��ִ� �� �ƴ϶�
	this->position.y -= 0;
	this->scale = Vector3(20.75, 20.75, 20.75);
	this->rotate = Vector3(0.0, 1.0, 0.0);
	this->boundingOffset = 1.5f;//1.8;
	this->color = glm::vec3(0.3, 0.3, 0.3);
}

void Door::DrawObject(GLuint s_program) {
	//this->color = glm::vec3(uidColor_block(dreColor_block), uidColor_block(dreColor_block), uidColor_block(dreColor_block));
	//cout << "DrawObject : Block" << endl; 
	glm::mat4 STR = glm::mat4(1.0f); //--- transformation matrix
	glm::mat4 R = glm::mat4(1.0f); //--- rotation matrix
	glm::mat4 T = glm::mat4(1.0f); //--- transformation matrix
	glm::mat4 S = glm::mat4(1.0f);
	T = glm::translate(T, this->position.GetGlmVec3()); //--- x������ translation
	R = glm::rotate(R, glm::radians(0.0f), this->rotate.GetGlmVec3()); //--- z�࿡���Ͽ� ȸ��
	S = glm::scale(glm::mat4(1.0f), this->scale.GetGlmVec3());
	STR = T * S * R; //--- �ռ� ��ȯ ���: translate -> rotate

	Vector3 lightColor = InGameManager::GetInstance().GetLightColor();
	glm::vec3 cameraPos = InGameManager::GetInstance().GetCameraPos();
	glm::vec3 cameraDirection = InGameManager::GetInstance().GetCameraDirection();
	glm::vec3 cameraUp = InGameManager::GetInstance().GetCameraUp();
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), WINDOW_WITDH / (float)WINDOW_HEIGHT, 0.001f, 1000.f);

	unsigned int modelLocation = glGetUniformLocation(s_program, "g_modelTransform"); //--- ���ؽ� ���̴������� ��ȯ ��ġ ��������
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(STR)); //--- modelTransform ������ ��ȯ �� �����ϱ�

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(s_program, "g_view");	// ���ؽ� ���̿��� viewTransform ������ġ
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);	// viewTransform ������ ��ȯ�� �����ϱ�

	unsigned int projectionLocation = glGetUniformLocation(s_program, "g_projection");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	unsigned int flaglocation = glGetUniformLocation(s_program, "flag");
	glUniform1i(flaglocation, 2);

	int lightPosLocation = glGetUniformLocation(s_program, "g_lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, lightPos.x, lightPos.y, lightPos.z);

	int lightColorLocation = glGetUniformLocation(s_program, "g_lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, lightColor.x, lightColor.y, lightColor.z);

	int objColorLocation = glGetUniformLocation(s_program, "g_objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
	glUniform3f(objColorLocation, this->color.x, this->color.y, this->color.z);

	int ViewLocation = glGetUniformLocation(s_program, "g_cameraPos");
	glUniform3f(ViewLocation, cameraPos.x, cameraPos.y, cameraPos.z);

	// ����� VAO �ҷ�����
	glBindVertexArray(InGameManager::GetInstance().GetVAO(this->type));
	// �ﰢ�� �׸���
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, InGameManager::GetInstance().GetTexture(TextureType::INGAME));
	glDrawElements(GL_TRIANGLES, InGameManager::GetInstance().GetObjData(this->type)->indexCount, GL_UNSIGNED_INT, 0);
}
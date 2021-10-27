#include "Standard.h"
#include "InGameManager.h"
#include "StartSceneUI.h"

StartSceneUI::StartSceneUI()
{
	this->type = TextureType::LOBBY;
	this->objectType = ObjectType::TEXTURE;
}

void StartSceneUI::DrawTextureImage(GLuint s_program) {
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.5f);
	glm::vec3 cameraDir = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.5f);

	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 V = glm::mat4(1.0f);
	glm::mat4 P = glm::mat4(1.0f);

	T = glm::translate(T, glm::vec3(-0.6f, 0.0f, 0.0f));

	unsigned int Line_Location = glGetUniformLocation(s_program, "g_modelTransform");
	glUniformMatrix4fv(Line_Location, 1, GL_FALSE, glm::value_ptr(T));

	unsigned int vLine_Location = glGetUniformLocation(s_program, "g_view");
	V = glm::lookAt(cameraPos, cameraDir, cameraUp);
	glUniformMatrix4fv(vLine_Location, 1, GL_FALSE, glm::value_ptr(V));

	unsigned int pLine_Location = glGetUniformLocation(s_program, "g_projection");
	P = glm::perspective(glm::radians(60.0f), (float)WINDOW_WITDH / WINDOW_HEIGHT, 0.1f, 200.0f);
	// P = glm::translate(P, glm::vec3(0.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(pLine_Location, 1, GL_FALSE, glm::value_ptr(P));

	glm::mat4 TestT = glm::mat4(1.0f);
	unsigned int Test = glGetUniformLocation(s_program, "g_modelTransform");
	glUniformMatrix4fv(Test, 1, GL_FALSE, glm::value_ptr(TestT));

	unsigned int flaglocation = glGetUniformLocation(s_program, "flag");
	glUniform1i(flaglocation, 1);

	int lightPosLocation = glGetUniformLocation(s_program, "g_lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, 0,0,0);

	int lightColorLocation = glGetUniformLocation(s_program, "g_lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, 1,1,1);

	int objColorLocation = glGetUniformLocation(s_program, "g_objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(objColorLocation, 1,1,1);

	glBindVertexArray(InGameManager::GetInstance().GetVAO(this->objectType));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, InGameManager::GetInstance().GetTexture(this->type));
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

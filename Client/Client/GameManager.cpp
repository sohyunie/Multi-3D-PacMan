#include "Standard.h"
#include "InGameManager.h"
#include "Player.h"
#include "NetworkManager.h"

void drawScene();
void drawSceneSubWindow();

using namespace std;

int subMainWindow;
int subWindow;
int mainWindow;

void InitBuffer()
{
	InGameManager::GetInstance().InitBuffer();
}

void InitShader()
{
	InGameManager::GetInstance().InitShader();
}

void DrawScene() {
	drawScene();
	//drawSceneSubWindow();
}

GLvoid drawScene()
{
	glutSetWindow(mainWindow);
	glClearColor(0, 0, 0, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	InGameManager::GetInstance().DrawScene();
	glutPostRedisplay();
	glutSwapBuffers();
}

GLvoid drawSceneSubWindow()
{
	glutSetWindow(subWindow);
	glClearColor(0, 0, 0, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	InGameManager::GetInstance().DrawSubScene();
	glutPostRedisplay();
	glutSwapBuffers();
}

void setProjection(int w, int h)
{
	float ratio;

	ratio = 1.0f * w / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h); // 윈도우 뷰포트

	gluPerspective(45, ratio, 0.1, 1000); // 1. 클리핑 공간 설정 원근투영
										  //glTranslatef(0.0, 1.5, -3.0); // 시야 확보

										  //glOrtho(0.0, w, 0.0, h, -1, 1); // 2. 클리핑 공간 설정 : 직각 투영

	glMatrixMode(GL_MODELVIEW);
} // Reshape == 화면 호출 때만 부르는거기에 한 번 만해도 족한 것들

GLvoid Reshape(int w, int h)
{
	//glViewport(0, 0, w, h);

	//glutSetWindow(subWindow);
	//glutPositionWindow(0, 0);
	//glutReshapeWindow(300, 300);
	//cout << "Reshape" << endl;
	//setProjection(300, 300);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	float cameraSpeed = 0.05f;
	Vector3 playerPos;
	switch (key)
	{
	case ((char)13):	// enter key
		if (InGameManager::GetInstance().GetState() == GAMESTATE::LOBBY)
		{
			InGameManager::GetInstance().PlayingBgm(SOUND_FILE_NAME_INGAME);
			InGameManager::GetInstance().SetState(GAMESTATE::INGAME);
		}
		break;
	case '1':
		InGameManager::GetInstance().SetFPS(true);
		break;
	case '3':
		InGameManager::GetInstance().SetFPS(false);
		break;
	case ' ':	// space bar
		if (InGameManager::GetInstance().GetState() == GAMESTATE::GAMEOVER ||
			InGameManager::GetInstance().GetState() == GAMESTATE::CLEAR) {
			InGameManager::GetInstance().InitGame();
			InGameManager::GetInstance().SetState(GAMESTATE::INGAME);
		}
		break;
	case((char)27):
		if (InGameManager::GetInstance().GetState() == GAMESTATE::GAMEOVER ||
			InGameManager::GetInstance().GetState() == GAMESTATE::CLEAR) {
			InGameManager::GetInstance().SetState(GAMESTATE::LOBBY);
			break;
		}
		else {
			exit(0);
		}
		break;
	}

	glutPostRedisplay();
}

void releaseKey(int key, int x, int y) {

	switch (key)
	{
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT: InGameManager::GetInstance().GetPlayer()->deltaAngle = 0; break;

	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN: InGameManager::GetInstance().GetPlayer()->deltaMove = 0; break;
	}
}

void processSpecialKeys(int key, int x, int y)
{
	Vector3 playerPos = Vector3();
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		switch (InGameManager::GetInstance().GetPlayer()->newDirection) {
		case Direction::DIR_NONE:
			InGameManager::GetInstance().GetPlayer()->newDirection = Direction::UP;
			break;
		case Direction::UP:
			InGameManager::GetInstance().GetPlayer()->newDirection = Direction::LEFT;
			break;
		case Direction::LEFT:
			InGameManager::GetInstance().GetPlayer()->newDirection = Direction::DOWN;
			break;
		case Direction::DOWN:
			InGameManager::GetInstance().GetPlayer()->newDirection = Direction::RIGHT;
			break;
		case Direction::RIGHT:
			InGameManager::GetInstance().GetPlayer()->newDirection = Direction::UP;
			break;
		}
		break;
	case GLUT_KEY_LEFT:
		switch (InGameManager::GetInstance().GetPlayer()->newDirection) {
		case Direction::DIR_NONE:
			InGameManager::GetInstance().GetPlayer()->newDirection = Direction::UP;
			break;
		case Direction::UP:
			InGameManager::GetInstance().GetPlayer()->newDirection = Direction::RIGHT;
			break;
		case Direction::RIGHT:
			InGameManager::GetInstance().GetPlayer()->newDirection = Direction::DOWN;
			break;
		case Direction::DOWN:
			InGameManager::GetInstance().GetPlayer()->newDirection = Direction::LEFT;
			break;
		case Direction::LEFT:
			InGameManager::GetInstance().GetPlayer()->newDirection = Direction::UP;
			break;
		}
		break;
	}

	glutPostRedisplay();
}

void TimerFunction(int value) {
	InGameManager::GetInstance().TimerFunction();

	switch (InGameManager::GetInstance().GetState())
	{
	case GAMESTATE::NONE:
		if (NetworkManager::GetInstance().GetIsConnected())
		{
			InGameManager::GetInstance().SetState(GAMESTATE::LOBBY);
		}
		break;
	}

	glutTimerFunc(10, TimerFunction, 1);
}

int main(int argc, char** argv)
{
	// Network Thread
	thread networkTherad(&NetworkManager::Network, &NetworkManager::GetInstance());	// 우리가 쓸 함수, 우리가 쓸 함수가 어떤 애의 건지 객체를 보여줌. 

	//PlaySound(TEXT(SOUND_FILE_NAME_LOBBY), NULL, SND_ASYNC | SND_LOOP);
	InGameManager::GetInstance().PlayingBgm(SOUND_FILE_NAME_LOBBY);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WITDH, WINDOW_HEIGHT);
	mainWindow = glutCreateWindow("Example1");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	InGameManager::GetInstance().InitObject();
	InitShader();
	InitBuffer();


	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutKeyboardFunc(Keyboard);
	glutSpecialUpFunc(releaseKey);
	glutSpecialFunc(processSpecialKeys);
	glutTimerFunc(50, TimerFunction, 1);

	//// SubWindow
	//subMainWindow = glutCreateSubWindow(mainWindow, WINDOW_WITDH, WINDOW_HEIGHT, WINDOW_WITDH, WINDOW_HEIGHT);
	//glutDisplayFunc(drawSceneSubWindow);

	// SubWindow
	subWindow = glutCreateSubWindow(mainWindow, 0,0, 300, 120);
	glutDisplayFunc(drawSceneSubWindow);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glutKeyboardFunc(Keyboard);
	glutSpecialUpFunc(releaseKey);
	glutSpecialFunc(processSpecialKeys);


	glutMainLoop();
}

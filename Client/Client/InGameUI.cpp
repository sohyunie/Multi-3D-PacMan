#include "Standard.h"
#include "InGameUI.h"
#include "InGameManager.h"

InGameUI::InGameUI() {

}

void InGameUI::PrintInGameUI(GLuint s_program) {
        if (this->isPlayGame == true) {
            // Time UI
            glClearColor(0.0, 0.0, 0.0, 0.0);
            string text = "TIME : " + to_string(InGameManager::GetInstance().currentTime()).substr(0, 4);
            const char* stringTime = text.data();
            glColor3f(1.0f, 1.0f, 1.0f);
            glRasterPos2f(-0.9, 0.66);  // 문자 출력할 위치 설정

            int len = (int)strlen(stringTime);
            for (int i = 0; i < len; i++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, stringTime[i]);
            }
            
            // Bead UI
             string beadText = "Bead : " + to_string(InGameManager::GetInstance().GetBeadCount());
             const char* stringBeadNum = beadText.data();

             glColor3f(1.0f, 1.0f, 1.0f);
             glRasterPos2f(-0.9, 0.33);  // 문자 출력할 위치 설정

             len = (int)strlen(stringBeadNum);
             for (int i = 0; i < len; i++) {
                 glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, stringBeadNum[i]);
             }

             // Hp UI
             string HpText = "HP : " + to_string(InGameManager::GetInstance().GetPlayerHP());
             const char* stringHp = HpText.data();
             glColor3f(1.0f, 1.0f, 1.0f);
             glRasterPos2f(-0.9, 0);  // 문자 출력할 위치 설정

             len = (int)strlen(stringHp);
             for (int i = 0; i < len; i++) {
                 glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, stringHp[i]);
             }

             // nearby Ghost UI
             //string nearbyText = "nearby ghost: " + InGameManager::GetInstance().GetNearByGhost().substr(0, 6);
             //const char* nearby = nearbyText.data();
             //glColor3f(1.0f, 1.0f, 1.0f);
             //glRasterPos2f(-0.9, -0.33);  // 문자 출력할 위치 설정

             //len = (int)strlen(nearby);
             //for (int i = 0; i < len; i++) {
             //    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, nearby[i]);
             //}

             // BestRecord UI
             string recordText = "Best Record : " + InGameManager::GetInstance().GetBestRecord().substr(0, 6);
             const char* recordHp = recordText.data();
             glColor3f(1.0f, 1.0f, 1.0f);
             glRasterPos2f(-0.9, -0.66);  // 문자 출력할 위치 설정

             len = (int)strlen(recordHp);
             for (int i = 0; i < len; i++) {
                 glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, recordHp[i]);
             }
        }
        //else {
        //    string text = "SPACE BAR를 누르면 재시작 가능합니다.";
        //    const char* string = text.data();
        //    glColor3f(0.5f, 0.5f, 0.5f);
        //    glRasterPos2f(-0.2, -0.2);  // 문자 출력할 위치 설정

        //    int len = (int)strlen(string);
        //    for (int i = 0; i < len; i++) {
        //        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
        //    }
        //    if (isClear) {
        //        text = "GAME CLEAR";
        //        const char* string = text.data();
        //        glColor3f(0.5f, 0.5f, 0.5f);
        //        glRasterPos2f(-0.2, 0.0);  // 문자 출력할 위치 설정

        //        int len = (int)strlen(string);
        //        for (int i = 0; i < len; i++) {
        //            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
        //        }
        //    }
        //    else {
        //        text = "GAME OVER";
        //        const char* string = text.data();
        //        glColor3f(0.5f, 0.5f, 0.5f);
        //        glRasterPos2f(-0.2, 0.0);  // 문자 출력할 위치 설정

        //        int len = (int)strlen(string);
        //        for (int i = 0; i < len; i++) {
        //            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
        //        }
        //    }
        //}
}
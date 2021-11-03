#include "Standard.h"
#include "InGameManager.h"
#include "Block.h"
#include "Bead.h"
#include "PowerBead.h"
#include "MapLoader.h"
#include "Player.h"

MapLoader::MapLoader(int map) {
    this->Loadfile(map);
}

GLvoid MapLoader::Loadfile(int map)
{
    FILE* fp = new FILE();
    switch (map)
    {
    case 0:
        fp = fopen("MAP_1.txt", "rt");
        break;
    case 1:
        fp = fopen("CLEAR_MAP.txt", "rt");
        break;
    case 2:
        fp = fopen("GAMEOVER_MAP.txt", "rt");
        break;
    case 3:
        fp = fopen("MAP_4.txt", "rt");
        break;
    case 4:
        fp = fopen("MAP_5.txt", "rt");
        break;
    }

    if (fp == NULL)
    {
        printf("\n board gen fail\n");
        return;
    }
    
    int cha;

    while (feof(fp) == 0)
    {
        for (int i = 0; i < MAP_SIZE; i++)
        {
            for (int j = 0; j < MAP_SIZE; j++)
            {
                fscanf(fp, "%d", &cha);
                BOARD_TYPE type = (BOARD_TYPE)cha;

                Vector3 position = Vector3((i * 7.5f - 35), 0, (j * 7.5f - 35));
                switch (type) {
                case BOARD_TYPE::BEAD_ITEM:
                    boardShape[i][j] = new Bead(position);
                    InGameManager::GetInstance().CountBeadAmount();
                    //InGameManager::GetInstance().CalculatePointLight(*boardShape[i][j], 10);
                    break;
                case BOARD_TYPE::POWERBEAD_ITEM:
                    boardShape[i][j] = new PowerBead(position);
                    break;
                case BOARD_TYPE::WALL_0:
                    boardShape[i][j] = new Block(position);
                    break;
                case BOARD_TYPE::NONE:
                    boardShape[i][j] = new StaticObject(position);
                    break;
                case BOARD_TYPE::INIT_PLAYER_POS:
                    InGameManager::GetInstance().GetPlayer()->InitPlayerPos(i, j, position);
                    boardShape[i][j] = new StaticObject(position);
                    break;
                case BOARD_TYPE::INIT_GHOST_POS:
                    InGameManager::GetInstance().CreateGhost(i, j, position);
                    InGameManager::GetInstance().CreateGhost(i, j, position);
                    InGameManager::GetInstance().CreateGhost(i, j, position);
                    boardShape[i][j] = new StaticObject(position);
                    //InGameManager::GetInstance().GetPlayer()->InitPlayerPos(i, j, position);
                    //boardShape[i][j] = new StaticObject(position);
                    break;
                }
            }
        }
    }

    fclose(fp);

    return;
}

GLvoid MapLoader::DrawMap(GLuint s_program) { 
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            this->boardShape[i][j]->DrawObject(s_program);
        }
    }
}

GLvoid MapLoader::DrawMapSecondFloor(GLuint s_program) {
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            this->boardShape[i][j]->DrawObject(s_program);
        }
    }
}
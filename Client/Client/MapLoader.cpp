#include "Standard.h"
#include "InGameManager.h"
#include "Block.h"
#include "Bead.h"
#include "Door.h"
#include "PowerBead.h"
#include "MapLoader.h"
#include "Player.h"

MapLoader::MapLoader(char mapArray[][30])
{
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < MAP_SIZE; j++)
        {
            int mapIndex = (int)mapArray[29-i][29-j] - 48;

            Vector3 position = Vector3(((j) * 7.5f - 35), 0, ((i) * 7.5f - 35));
            switch (mapIndex) {
            case 0: // BEAD
                boardShape[i][j] = new Bead(position);
                InGameManager::GetInstance().CountBeadAmount();
                break;
            case 1: // KEY
                boardShape[i][j] = new PowerBead(position);
                break;
            case 2: // WALL
                boardShape[i][j] = new Block(position);
                break;
            case 3: // NONE
                boardShape[i][j] = new StaticObject(position);
                break;
            case 4: // DOOR
                boardShape[i][j] = new Door(position);
                break;
            //case BOARD_TYPE::INIT_PLAYER_1:
            //    InGameManager::GetInstance().GetPlayer()->InitPlayerPos(i, j, position);
            //    boardShape[i][j] = new StaticObject(position);
            //    break;
            //case BOARD_TYPE::INIT_PLAYER_2:
            //    InGameManager::GetInstance().CreateGhost(i, j, position);
            //    boardShape[i][j] = new StaticObject(position);
            //    break;
            //case BOARD_TYPE::INIT_GHOST_POS:
            //    InGameManager::GetInstance().CreateGhost(i, j, position);
            //    boardShape[i][j] = new StaticObject(position);
            //    //InGameManager::GetInstance().GetPlayer()->InitPlayerPos(i, j, position);
            //    //boardShape[i][j] = new StaticObject(position);
            //    break;
            }
        }
    }
}


GLvoid MapLoader::DrawMap(GLuint s_program) { 
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            this->boardShape[i][j]->DrawObject(s_program);
        }
    }
}

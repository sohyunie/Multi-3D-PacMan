#pragma once
class StartSceneUI
{
public:
    StartSceneUI();
    void DrawTextureImage(GLuint s_program);
private :
    TextureType type;
    ObjectType objectType;
};


#pragma once
class EndingScene
{
public:
    EndingScene();
    void DrawTextureImage(GLuint s_program, TextureType type);
private:
    TextureType type;
    ObjectType objectType;
};


#include "Standard.h"
#include "Object.h"

bool Object::CollisionCheck(Object type) {
    Vector4 mybbox = this->GetBoundingBox();
    Vector4 otherbbox = type.GetBoundingBox();
    bool xcollision = false;
    bool zcollision = false;

    if ((mybbox.minX <= otherbbox.minX && mybbox.maxX >= otherbbox.minX) ||
        ((mybbox.minX <= otherbbox.maxX && mybbox.maxX >= otherbbox.maxX))) {
        xcollision = true;
    }

    if ((mybbox.minZ <= otherbbox.minZ && mybbox.maxZ >= otherbbox.minZ) ||
        ((mybbox.minZ <= otherbbox.maxZ && mybbox.maxZ >= otherbbox.maxZ))) {
        zcollision = true;
    }

    if (xcollision && zcollision) {
        return true;    // 충돌함
    }
    else
        return false;   // 충돌안함
}


Vector4 Object::GetBoundingBox() {
    return Vector4(position.x - boundingOffset, position.z - boundingOffset, position.x + boundingOffset, position.z + boundingOffset);
}

void Object::DrawObject(GLuint s_program) {
    cout << "DrawObject : Object" << endl;
}
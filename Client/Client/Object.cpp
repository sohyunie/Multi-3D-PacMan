#include "Standard.h"
#include "Object.h"

bool Object::CollisionCheck(Object type) {
    CVector4 mybbox = this->GetBoundingBox();
    CVector4 otherbbox = type.GetBoundingBox();
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
        return true;    // �浹��
    }
    else
        return false;   // �浹����
}


CVector4 Object::GetBoundingBox() {
    return CVector4(position.x - boundingOffset, position.z - boundingOffset, position.x + boundingOffset, position.z + boundingOffset);
}

void Object::DrawObject(GLuint s_program) {
    cout << "DrawObject : Object" << endl;
}
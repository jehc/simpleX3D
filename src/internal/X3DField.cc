#include "internal/X3DField.h"

namespace X3D {

const string X3DField::typeNames[] = {
    "SFBool",
    "SFColor",
    "SFColorRGBA",
    "SFDouble",
    "SFFloat",
    "SFImage",
    "SFInt32",
    "SFMatrix3d",
    "SFMatrix3f",
    "SFMatrix4d",
    "SFMatrix4f",
    "SFNode",
    "SFRotation",
    "SFString",
    "SFTime",
    "SFVec2d",
    "SFVec2f",
    "SFVec3d",
    "SFVec3f",
    "SFVec4d",
    "SFVec4f",
    "MFBool",
    "MFColor",
    "MFColorRGBA",
    "MFDouble",
    "MFFloat",
    "MFImage",
    "MFInt32",
    "MFMatrix3d",
    "MFMatrix3f",
    "MFMatrix4d",
    "MFMatrix4f",
    "MFNode",
    "MFRotation",
    "MFString",
    "MFTime",
    "MFVec2d",
    "MFVec2f",
    "MFVec3d",
    "MFVec3f",
    "MFVec4d",
    "MFVec4f"
};

const string& X3DField::getTypeName(X3DField::Type type) {
    return X3DField::typeNames[type];
}

}

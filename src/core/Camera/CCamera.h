#ifndef ATOM_CAMERA_H
#define ATOM_CAMERA_H

#include "../../Defines.h"
#include "../Components.h"
#include "../System.h"

namespace atom
{
	
struct CCamera
{
    v3 Forward;
    v3 Right;
    v3 Up;

    m4 View;
    m4 Projection;

    f32 FOV;
    f32 Near;
    f32 Far;

    f32 Pitch;
    f32 Yaw;
    f32 Roll;
};

}

#endif // ATOM_CAMERA_H
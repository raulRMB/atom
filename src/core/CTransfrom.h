#ifndef ATOM_CTRANSFORM
#define ATOM_CTRANSFORM

#include "Components.h"
#include "../Defines.h"

namespace atom
{

struct CTransform : public CComponent
{
	v3 Position;
	v3 Rotation;
	v3 Scale;
};

}

#endif // !ATOM_CTRANSFORM

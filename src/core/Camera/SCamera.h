#ifndef ATOM_SCAMERA_H
#define ATOM_SCAMERA_H

#include "../../Defines.h"
#include "../System.h"
#include <entt/entt.hpp>

namespace atom
{

enum ECameraMovementFlags : u8
{
	None		= 0x0,
	Forward		= 0x1,
	Backward	= 0x2,
	Left		= 0x4,
	Right		= 0x8,
	Up			= 0x10,
	Down		= 0x20,
};

class SCamera : public SUpdateSystem
{
	entt::entity m_CameraEntity;
	u8 m_MovementFlags;
public:
	SCamera();
	~SCamera() = default;

	void Update(f32 dt) override;

	void MoveCameraForward();
	void MoveCameraBackward();
	void MoveCameraLeft();
	void MoveCameraRight();

	void StopCameraForward();
	void StopCameraBackward();
	void StopCameraLeft();
	void StopCameraRight();

	void OnMouseMoved(v2 delta);
};

} // namespace atom

#endif // ATOM_SCAMERA_H
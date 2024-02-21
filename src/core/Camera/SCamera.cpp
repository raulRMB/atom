#include "SCamera.h"
#include "../CTransfrom.h"
#include "CCamera.h"
#include "../Engine.h"
#include "../InputHandler.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/ext.hpp>

namespace atom
{

SCamera::SCamera() : m_MovementFlags(0), m_CameraEntity(entt::null)
{
	m_CameraEntity = Engine::CreateEntity();

	CTransform transform{};
	transform.Position = { 0.0f, 0.0f, 3.0f };
	transform.Rotation = { 0.0f, 0.0f, 0.0f };
	transform.Scale = { 1.0f, 1.0f, 1.0f };	

	Engine::AddComponent(m_CameraEntity, transform);

	CCamera cam{};
	cam.FOV = 45.0f;
	cam.Near = 0.1f;
	cam.Far = 100.0f;
	cam.Forward = v3(0.0f, 0.0f, -1.0f);
	cam.Right = v3(1.0f, 0.0f, 0.0f);
	cam.Up = v3(0.0f, 1.0f, 0.0f);
	cam.Pitch = -180.f;
	cam.Yaw = 90.0f;
	cam.Roll = 0.0f;
	cam.Projection = glm::perspective(glm::radians(cam.FOV), 640.0f / 480.0f, cam.Near, cam.Far);
	//cam.View = glm::lookAt(transform.Position, transform.Position + cam.Forward, cam.Up);
	cam.View = glm::lookAt(transform.Position, v3(0.f), v3(0., 1., 0.));

	Engine::AddComponent(m_CameraEntity, cam);

	InputHandler::RegisterOnKeyPressed<&SCamera::MoveCameraForward>(KeyCode::W, this);
	InputHandler::RegisterOnKeyPressed<&SCamera::MoveCameraBackward>(KeyCode::S, this);
	InputHandler::RegisterOnKeyPressed<&SCamera::MoveCameraLeft>(KeyCode::A, this);
	InputHandler::RegisterOnKeyPressed<&SCamera::MoveCameraRight>(KeyCode::D, this);
	InputHandler::RegisterOnKeyPressed<&SCamera::MoveCameraUp>(KeyCode::Space, this);
    InputHandler::RegisterOnKeyPressed<&SCamera::MoveCameraDown>(KeyCode::LeftControl, this);

    InputHandler::RegisterOnKeyReleased<&SCamera::StopCameraForward>(KeyCode::W, this);
	InputHandler::RegisterOnKeyReleased<&SCamera::StopCameraBackward>(KeyCode::S, this);
	InputHandler::RegisterOnKeyReleased<&SCamera::StopCameraLeft>(KeyCode::A, this);
	InputHandler::RegisterOnKeyReleased<&SCamera::StopCameraRight>(KeyCode::D, this);
    InputHandler::RegisterOnKeyReleased<&SCamera::StopCameraUp>(KeyCode::Space, this);
    InputHandler::RegisterOnKeyReleased<&SCamera::StopCameraDown>(KeyCode::LeftControl, this);

	//InputHandler::RegisterOnMouseMovedDeltaVector<&SCamera::OnMouseMoved>(this);
}

void SCamera::Update([[maybe_unused]] f32 dt)
{
	v3 movement = v3(0.0f);
	CCamera& cam = Engine::GetComponent<CCamera>(m_CameraEntity);

	if(m_MovementFlags & ECameraMovementFlags::Forward)
	{
		movement += cam.Forward;
	}
	if(m_MovementFlags & ECameraMovementFlags::Backward)
	{
		movement -= cam.Forward;
	}
	if (m_MovementFlags & ECameraMovementFlags::Right)
	{
		movement += cam.Right;
	}
	if(m_MovementFlags & ECameraMovementFlags::Left)
	{
		movement -= cam.Right;
	}
    if(m_MovementFlags & ECameraMovementFlags::Up)
    {
        movement += cam.Up;
    }
    if(m_MovementFlags & ECameraMovementFlags::Down)
    {
        movement -= cam.Up;
    }

	CTransform& transform = Engine::GetComponent<CTransform>(m_CameraEntity);

	transform.Position += movement * 10.f * dt;

	v3 front;
	front.x = cos(glm::radians(cam.Yaw)) * cos(glm::radians(cam.Pitch));
	front.y = sin(glm::radians(cam.Pitch));
	front.z = sin(glm::radians(cam.Yaw)) * cos(glm::radians(cam.Pitch));
	cam.Forward = glm::normalize(front);

	cam.Right = glm::normalize(glm::cross(cam.Forward, v3(0.0f, 1.0f, 0.0f)));
	cam.Up = glm::normalize(glm::cross(cam.Right, cam.Forward));

	//cam.View = glm::lookAt(transform.Position, transform.Position + cam.Forward, cam.Up);
	cam.View = glm::lookAt(transform.Position, v3(0.f), v3(0.0f, 1.0f, 0.0f));
}

void SCamera::MoveCameraForward()
{
	m_MovementFlags |= ECameraMovementFlags::Forward;
}

void SCamera::MoveCameraBackward()
{
	m_MovementFlags |= ECameraMovementFlags::Backward;
}

void SCamera::MoveCameraLeft()
{
	m_MovementFlags |= ECameraMovementFlags::Left;
}

void SCamera::MoveCameraRight()
{
	m_MovementFlags |= ECameraMovementFlags::Right;
}

void SCamera::MoveCameraUp()
{
    m_MovementFlags |= ECameraMovementFlags::Up;
}

void SCamera::MoveCameraDown()
{
    m_MovementFlags |= ECameraMovementFlags::Down;
}

void SCamera::StopCameraForward()
{
	m_MovementFlags &= ~ECameraMovementFlags::Forward;
}

void SCamera::StopCameraBackward()
{
	m_MovementFlags &= ~ECameraMovementFlags::Backward;
}

void SCamera::StopCameraLeft()
{
	m_MovementFlags &= ~ECameraMovementFlags::Left;
}

void SCamera::StopCameraRight()
{
	m_MovementFlags &= ~ECameraMovementFlags::Right;
}

void SCamera::StopCameraUp()
{
    m_MovementFlags &= ~ECameraMovementFlags::Up;
}

void SCamera::StopCameraDown()
{
    m_MovementFlags &= ~ECameraMovementFlags::Down;
}

void SCamera::OnMouseMoved(v2 delta)
{
	CCamera& cam = Engine::GetComponent<CCamera>(m_CameraEntity);
	CTransform& transform = Engine::GetComponent<CTransform>(m_CameraEntity);

	LogInfo("Delta x: %f", delta.x);
	LogInfo("Delta y: %f", delta.y);

	cam.Yaw += delta.x * 5.f;
	cam.Pitch += delta.y * 5.f;

	if(cam.Pitch > 89.0f)
	{
		cam.Pitch = 89.0f;
	}
	if(cam.Pitch < -89.0f)
	{
		cam.Pitch = -89.0f;
	}

	// LogInfo("Forward: %f, %f, %f", cam.Forward.x, cam.Forward.y, cam.Forward.z);
}


} // namespace atom

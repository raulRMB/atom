#include "InputHandler.h"

#include <GLFW/glfw3.h>

namespace atom
{

InputHandler& InputHandler::GetInstance()
{
	static InputHandler instance;
	return instance;
}

void InputHandler::HandleKeyInput(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
{
	if (action == GLFW_PRESS)
	{
		OnKeyPressed(key);
	}
	else if (action == GLFW_RELEASE)
	{
		OnKeyReleased(key);
	}
}

void InputHandler::HandleMousePosChange(GLFWwindow* window, f64 xpos, f64 ypos)
{
	v2 currentMousePos = { xpos, ypos };
	v2 delta = currentMousePos - m_LastMousePos;
	m_LastMousePos = currentMousePos;

	m_OnMouseMovedCurrentXY.publish(xpos, ypos);
	m_OnMouseMovedDeltaVector.publish(delta);
}

InputHandler::InputHandler()
{
}

void InputHandler::OnKeyPressed(i32 key)
{
	m_KeyStates[key] = true;
	m_OnKeyPressedSignal[key].publish();
}

void InputHandler::OnKeyReleased(i32 key)
{
	m_KeyStates[key] = false;
	m_OnKeyReleasedSignal[key].publish();
}

} // namespace atom
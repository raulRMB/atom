#ifndef ATOM_INPUTHANDLER_H
#define ATOM_INPUTHANDLER_H

#include "../Defines.h"
#include "entt/entt.hpp"

#include <GLFW/glfw3.h>

namespace atom
{

enum class KeyCode
{
	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_N,
	N = GLFW_KEY_M,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z,
	Count,
};

class InputHandler
{
private:
	bool m_KeyStates[GLFW_KEY_LAST] = {0};
	v2 m_LastMousePos = {0, 0};

	friend class Engine;
private:

	void HandleKeyInput(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
	void HandleMousePosChange(GLFWwindow* window, f64 xpos, f64 ypos);
	InputHandler();
	~InputHandler() = default;

	entt::sigh<void()> m_OnKeyPressedSignal[(u32)KeyCode::Count] = { {} };
	entt::sigh<void()> m_OnKeyReleasedSignal[(u32)KeyCode::Count] = { {} };

	entt::sigh<void(f64, f64)> m_OnMouseMovedCurrentXY{};
	entt::sigh<void(v2)> m_OnMouseMovedDeltaVector{};

	void OnKeyPressed(i32 key);
	void OnKeyReleased(i32 key);
public:
	static InputHandler& GetInstance();

	template<void (*func)()>
	static void RegisterOnKeyPressed(KeyCode keyCode)
	{
		entt::sink sink { GetInstance().m_OnKeyPressedSignal[(u32)keyCode] };
		sink.connect<func>();
	};

	template<auto Candidate, typename Type>
	static void RegisterOnKeyPressed(KeyCode keyCode, Type* value_or_instance) noexcept {
		entt::sink sink{ GetInstance().m_OnKeyPressedSignal[(u32)keyCode] };
		sink.connect<Candidate>(value_or_instance);
	}

	template<void (*func)()>
	static void RegisterOnKeyReleased(KeyCode keyCode)
	{
		entt::sink sink{ GetInstance().m_OnKeyReleasedSignal[(u32)keyCode] };
		sink.connect<func>();
	};

	template<auto Candidate, typename Type>
	static void RegisterOnKeyReleased(KeyCode keyCode, Type* value_or_instance) noexcept {
		entt::sink sink{ GetInstance().m_OnKeyReleasedSignal[(u32)keyCode] };
		sink.connect<Candidate>(value_or_instance);
	}

	template<void (*func)(f64, f64)>
	static void RegisterOnMouseMovedCurrentXY()
	{
		entt::sink sink{ GetInstance().m_OnMouseMovedCurrentXY };
		sink.connect<func>();
	};

	template<auto Candidate, typename Type>
	static void RegisterOnMouseMovedCurrentXY(Type* value_or_instance) noexcept {
		entt::sink sink{ GetInstance().m_OnMouseMovedCurrentXY };
		sink.connect<Candidate>(value_or_instance);
	}

	template<void (*func)(v2)>
	static void RegisterOnMouseMovedDeltaVector()
	{
		entt::sink sink{ GetInstance().m_OnMouseMovedDeltaVector };
		sink.connect<func>();
	};

	template<auto Candidate, typename Type>
	static void RegisterOnMouseMovedDeltaVector(Type* value_or_instance) noexcept {
		entt::sink sink{ GetInstance().m_OnMouseMovedDeltaVector };
		sink.connect<Candidate>(value_or_instance);
	}
};



} // namespace atom

#endif // ATOM_INPUTHANDLER_H
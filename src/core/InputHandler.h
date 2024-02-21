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
	Num0 = GLFW_KEY_0,
	Num1 = GLFW_KEY_1,
	Num2 = GLFW_KEY_2,
	Num3 = GLFW_KEY_3,
	Num4 = GLFW_KEY_4,
	Num5 = GLFW_KEY_5,
	Num6 = GLFW_KEY_6,
	Num7 = GLFW_KEY_7,
	Num8 = GLFW_KEY_8,
	Num9 = GLFW_KEY_9,
    Space  = GLFW_KEY_SPACE,
	Escape = GLFW_KEY_ESCAPE,
	Enter = GLFW_KEY_ENTER,
	Tab = GLFW_KEY_TAB,
	Backspace = GLFW_KEY_BACKSPACE,
	Insert = GLFW_KEY_INSERT,
	Delete = GLFW_KEY_DELETE,
	Right = GLFW_KEY_RIGHT,
	Left = GLFW_KEY_LEFT,
	Down = GLFW_KEY_DOWN,
	Up = GLFW_KEY_UP,
	PageUp = GLFW_KEY_PAGE_UP,
	PageDown = GLFW_KEY_PAGE_DOWN,
	Home = GLFW_KEY_HOME,
	End = GLFW_KEY_END,
	CapsLock = GLFW_KEY_CAPS_LOCK,
	ScrollLock = GLFW_KEY_SCROLL_LOCK,
	NumLock = GLFW_KEY_NUM_LOCK,
	PrintScreen = GLFW_KEY_PRINT_SCREEN,
	Pause = GLFW_KEY_PAUSE,
	F1 = GLFW_KEY_F1,
	F2 = GLFW_KEY_F2,
	F3 = GLFW_KEY_F3,
	F4 = GLFW_KEY_F4,
	F5 = GLFW_KEY_F5,
	F6 = GLFW_KEY_F6,
	F7 = GLFW_KEY_F7,
	F8 = GLFW_KEY_F8,
	F9 = GLFW_KEY_F9,
	F10 = GLFW_KEY_F10,
	F11 = GLFW_KEY_F11,
	F12 = GLFW_KEY_F12,
	Keypad0 = GLFW_KEY_KP_0,
	Keypad1 = GLFW_KEY_KP_1,
	Keypad2 = GLFW_KEY_KP_2,
	Keypad3 = GLFW_KEY_KP_3,
	Keypad4 = GLFW_KEY_KP_4,
	Keypad5 = GLFW_KEY_KP_5,
	Keypad6 = GLFW_KEY_KP_6,
	Keypad7 = GLFW_KEY_KP_7,
	Keypad8 = GLFW_KEY_KP_8,
	Keypad9 = GLFW_KEY_KP_9,
	KeypadDecimal = GLFW_KEY_KP_DECIMAL,
	KeypadDivide = GLFW_KEY_KP_DIVIDE,
	KeypadMultiply = GLFW_KEY_KP_MULTIPLY,
	KeypadSubtract = GLFW_KEY_KP_SUBTRACT,
	KeypadAdd = GLFW_KEY_KP_ADD,
	KeypadEnter = GLFW_KEY_KP_ENTER,
	KeypadEqual = GLFW_KEY_KP_EQUAL,
	LeftShift = GLFW_KEY_LEFT_SHIFT,
	LeftControl = GLFW_KEY_LEFT_CONTROL,
	LeftAlt = GLFW_KEY_LEFT_ALT,
	LeftSuper = GLFW_KEY_LEFT_SUPER,
	RightShift = GLFW_KEY_RIGHT_SHIFT,
	RightControl = GLFW_KEY_RIGHT_CONTROL,
	RightAlt = GLFW_KEY_RIGHT_ALT,
	RightSuper = GLFW_KEY_RIGHT_SUPER,
	Menu = GLFW_KEY_MENU,
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
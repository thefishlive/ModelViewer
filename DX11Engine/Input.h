#pragma once

#include "stdafx.h"

namespace DX11Engine
{
	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		bool Init(HINSTANCE instance, HWND window);
		bool Update();

		BYTE GetKeyState(BYTE key);

		bool IsDown(BYTE key);

		void Release();

	private:
		LPDIRECTINPUT8 m_dInput;

		IDirectInputDevice8* m_keyboard;
		IDirectInputDevice8* m_mouse;

		DIMOUSESTATE m_mouseLastState;
		BYTE m_lastKeyboardState[256];

		DIMOUSESTATE m_mouseCurrentState;
		BYTE m_keyboardState[256];
	};

	class InputAxis
	{
	public:

	private:

	};
}


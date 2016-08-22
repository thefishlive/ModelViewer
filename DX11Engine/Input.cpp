#include "Input.h"

DX11Engine::InputManager::InputManager()
{
}

DX11Engine::InputManager::~InputManager()
{
}

bool DX11Engine::InputManager::Init(HINSTANCE instance, HWND window)
{
	HRESULT result;

	// Create Direct Input instance
	result = DirectInput8Create(
		instance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_dInput,
		NULL
	);

	HR_B(result, TEXT("DirectInput8Create"));

	// Setup keyboard
	result = m_dInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	HR_B(result, TEXT("m_dInput->CreateDevice"));
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	HR_B(result, TEXT("m_keyboard->SetDataFormat"));
	result = m_keyboard->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	HR_B(result, TEXT("m_keyboard->SetCooperativeLevel"));

	// Setup mouse
	result = m_dInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	HR_B(result, TEXT("m_dInput->CreateDevice"));
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	HR_B(result, TEXT("m_mouse->SetDataFormat"));
	result = m_mouse->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	HR_B(result, TEXT("m_mouse->SetCooperativeLevel"));
	return true;
}

bool DX11Engine::InputManager::Update()
{
	m_mouseLastState = m_mouseCurrentState;
	memcpy(m_lastKeyboardState, m_keyboardState, sizeof(m_keyboardState));

	HRESULT result;

	result = m_keyboard->Acquire();

	// Fail silently, probably just minimised
	if (FAILED(result))
	{
		return true;
	}

	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	HR_B(result, TEXT("m_keyboard->GetDeviceState"));

	result = m_mouse->Acquire();

	// Fail silently, probably just minimised
	if (FAILED(result))
	{
		return true;
	}

	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseCurrentState);
	HR_B(result, TEXT("m_mouse->GetDeviceState"));

	return true;
}

BYTE DX11Engine::InputManager::GetKeyState(BYTE key)
{
	return m_keyboardState[key];
}

bool DX11Engine::InputManager::IsDown(BYTE key)
{
	return GetKeyState(key) & 0x80;
}

void DX11Engine::InputManager::Release()
{
	SAFE_RELEASE(m_keyboard);
	SAFE_RELEASE(m_mouse);

	SAFE_RELEASE(m_dInput);
}

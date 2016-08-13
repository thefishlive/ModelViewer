#include "Window.h"

using namespace DX11Engine;

DX11Engine::Window::Window(LPCTSTR name, int width, int height, bool fullscreen) :
	m_name(name),
	m_width(width),
	m_height(height),
	m_fullscreen(fullscreen),
	m_device(
		width,
		height,
		!fullscreen,
		Camera( 
			{2.5f, 2.5f, -2.5f, 0.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f, 0.0f},
			0.4f * XM_PI,
			(float) width / height,
			0.4f, 1000.0f
		)
	)
{
}

DX11Engine::Window::~Window()
{
}

bool DX11Engine::Window::Init(HINSTANCE instance, int ShowWnd)
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Window::WndProc;
	wc.cbClsExtra = NULL;
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_name;

	if (!m_fullscreen)
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);

	if (!RegisterClassEx(&wc))
		return false;

	RECT wr = { 0, 0, m_width, m_height };

	if (!m_fullscreen)
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	m_window = CreateWindowEx(
		NULL,
		m_name,
		m_name,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		instance,
		NULL
	);

	if (!m_window)
	{
		return false;
	}

	if (!m_device.InitDevice(instance, m_window))
	{
		MessageBox(0, L"Direct3D Initialization - Failed", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!m_device.InitScene())
	{
		MessageBox(0, L"Scene Initialization - Failed", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(m_window, ShowWnd);
	UpdateWindow(m_window);

	return true;
}

void DX11Engine::Window::SetFullscreen(bool fullscreen)
{
	m_fullscreen = fullscreen;
	m_device.SetFullscreen(fullscreen);
}

int DX11Engine::Window::Start()
{
	m_running = true;
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (m_running)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_QUIT:
				Stop();
				break;
			default:
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				break;
			}
		}
		else
		{
			m_timer.Update();

			if (!m_device.UpdateScene(m_timer.FrameTime))
			{
				MessageBox(0, L"Error updating scene", L"Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (!m_device.DrawScene())
			{
				MessageBox(0, L"Error drawing scene", L"Error", MB_OK | MB_ICONERROR);
				break;
			}
		}
	}
	return (int) msg.wParam;
}

void DX11Engine::Window::Stop()
{
	m_running = false;
}

LRESULT DX11Engine::Window::WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(window);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(window, msg, wParam, lParam);
}

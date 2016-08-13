#pragma once

#include "stdafx.h"
#include "DirectXDevice.h"
#include "FPSTimer.h"

namespace DX11Engine
{
	class Window
	{
	public:
		Window(LPCTSTR name, int width, int height, bool windowed);
		~Window();

		bool Init(HINSTANCE instance, int ShowWnd);

		void SetFullscreen(bool fullscren);

		int Start();
		void Stop();

		static LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		LPCTSTR m_name;
		int m_width;
		int m_height;

		bool m_fullscreen;
		bool m_running;

		HWND m_window;
		DirectXDevice m_device;

		FPSTimer m_timer;
	};
}


#include "Main.h"

using namespace DX11Engine;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR lpCmdLine, int nShowCmd)
{
	Window window = Window(L"DX11 Engine", 800, 800, true);

	if (!window.Init(instance, nShowCmd))
	{
		MessageBox(NULL, L"Could not initialize window", L"Error", MB_ICONERROR | MB_OK);
		return 1;
	}

	window.Start();
	return 0;
}

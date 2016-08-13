#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

// Remove uneeded sections of WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// macro to allow safe release of d3d resources
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

// Check the state of a HRESULT
#define CHECK_RESULT_VOID(r, t) { if (FAILED(r)) { MessageBox(NULL, t, TEXT("Direct 3D Error"), MB_ICONERROR | MB_OK); return; } }
#define CHECK_RESULT_BOOL(r, t) { if (FAILED(r)) { MessageBox(NULL, t, TEXT("Direct 3D Error"), MB_ICONERROR | MB_OK); return false; } }

#define SHADER_VERSION(t) t "_4_0"
#define VERTEX_SHADER_VERSION SHADER_VERSION("vs")
#define PIXEL_SHADER_VERSION SHADER_VERSION("ps")

#include <Windows.h>
#include <d3d11.h>
#include <d3dx10.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>

#include "ShaderStructures.h"

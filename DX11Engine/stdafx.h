#pragma once

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "dwrite.lib")

// Remove uneeded sections of WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Perform checks on HRESULTS
#define RESULT_CHECKS

// macro to allow safe release of d3d resources
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

// Check the state of a HRESULT
#define FORCE_CHECK_RESULT_VOID(r, t) { if (FAILED(r)) { MessageBox(NULL, t, TEXT("Direct 3D Error"), MB_ICONERROR | MB_OK); return; } }
#define FORCE_CHECK_RESULT_BOOL(r, t) { if (FAILED(r)) { MessageBox(NULL, t, TEXT("Direct 3D Error"), MB_ICONERROR | MB_OK); return false; } }

#ifdef RESULT_CHECKS
#define CHECK_RESULT_VOID(r, t) { FORCE_CHECK_RESULT_VOID(r, t) }
#define CHECK_RESULT_BOOL(r, t) { FORCE_CHECK_RESULT_BOOL(r, t) }
#else
#define CHECK_RESULT_VOID(r, t) { }
#define CHECK_RESULT_BOOL(r, t) { }
#endif

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

void CreateBuffer(ID3D11Device* device, ID3D11Buffer **target, UINT flags, const void* data, UINT size);


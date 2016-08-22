#pragma once

#include "stdafx.h"

#include <vector>

#include "VertexShader.h"
#include "PixelShader.h"
#include "Model.h"
#include "Camera.h"
#include "FontRenderer.h"
#include "Input.h"
#include "FPSTimer.h"


typedef const DX11Engine::Camera& CCamera;

namespace DX11Engine
{
	class DirectXDevice
	{
	public:
		DirectXDevice(int width, int height, BOOL windowed, CCamera camera, D3DXCOLOR background = { 0.0f, 0.0f, 0.0f, 1.0f });
		~DirectXDevice();

		bool InitDevice(HINSTANCE instance, HWND window);

		bool InitScene();
		bool UpdateScene(float time);
		bool DrawScene(FPSTimer timer);

		void SetFullscreen(bool fullscreen);

		bool Release();

		D3DXCOLOR Background;
		Camera Camera;

	private:
		HWND m_window;
		bool m_windowed;

		bool m_vsync;
		bool m_lightingEnabled = true;

		// The windowed width and height
		int m_width;
		int m_height;

		ID3D11Device* m_device;
		ID3D11DeviceContext* m_devcon;
		IDXGISwapChain* m_swapChain;
		ID3D11RenderTargetView* m_rtv;
		ID3D11InputLayout* m_layout;
		ID3D11DepthStencilView* m_depthStencil;
		ID3D11Texture2D* m_depthBuffer;
		ID3D11RasterizerState* m_renderState;

		LightBuffer m_lightBuffer;
		ID3D11Buffer* m_sceneBuffer;

		VertexShader m_vs;
		PixelShader m_ps;
		PixelShader m_unlit;

		std::vector<Model> m_models;

		FontRenderer m_fontRenderer;
		InputManager m_input;

		D3D11_VIEWPORT BuildViewport(int width, int height);
	};
}


#pragma once

#include "stdafx.h"

#include <vector>

#include "VertexShader.h"
#include "PixelShader.h"
#include "Model.h"

namespace DX11Engine
{
	class DirectXDevice
	{
	public:
		DirectXDevice(int width, int height, bool windowed, D3DXCOLOR background);
		~DirectXDevice();

		bool InitDevice(HINSTANCE instance, HWND window);

		bool InitScene();
		bool UpdateScene();
		bool DrawScene();

		void SetFullscreen(bool fullscreen);

		bool Release();

		D3DXCOLOR m_background;

	private:
		HWND m_window;
		bool m_windowed;

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

		VertexShader m_vs;
		PixelShader m_ps;

		Model m_model;
		std::vector<Model> m_models;

		D3D11_VIEWPORT BuildViewport(int width, int height);
	};
}


#pragma once

#include "stdafx.h"

namespace DX11Engine
{
	class FontRenderer
	{
	public:
		FontRenderer();
		~FontRenderer();

		bool Init(ID3D11Device* device, IDXGIAdapter1* adapter, int width, int height);
		bool InitScreenTexture(ID3D11Device* device);

		void PrintText(ID3D11DeviceContext* devcon, std::wstring text);

		void Release();

	private:
		ID3D10Device1 *m_device;

		ID2D1RenderTarget *m_renderTarget;
		ID2D1SolidColorBrush *m_brush;

		IDWriteFactory *m_dwriteFactory;
		IDWriteTextFormat *m_textFormat;

		ID3D11Buffer *m_vertBuffer;
		ID3D11Buffer *m_indexBuffer;
		ID3D11Buffer *m_objectBuffer;
		ID3D11Texture2D *m_backBuffer;
		ID3D11Texture2D *m_sharedTex;
		ID3D11ShaderResourceView *m_texture;

		ID3D11RasterizerState *m_renderState;
		ID3D11SamplerState *m_sampler;
		ID3D11BlendState *m_blendState;

		IDXGIKeyedMutex *m_lock11;
		IDXGIKeyedMutex *m_lock10;

		int m_width;
		int m_height;
	};
}


#pragma once

#include "stdafx.h"

namespace DX11Engine
{
	class Model
	{
	public:
		Model();
		~Model();

		bool Init(ID3D11Device* device, Vertex verticies[], UINT vCount, DWORD indicies[], UINT iCount, LPCTSTR texturename = L"", LPCTSTR normalMap = L"");
		bool Draw(ID3D11DeviceContext* devcon);
		void Release();

		bool Loaded;

		XMMATRIX Transformation;
		XMMATRIX WVP;

	private:
		ID3D11Buffer* m_verticies;
		ID3D11Buffer* m_indicies;
		ID3D11Buffer* m_objBuffer;

		ID3D11ShaderResourceView* m_texture;
		ID3D11ShaderResourceView* m_normalMap;
		ID3D11SamplerState* m_sampler;

		UINT m_vCount;
		UINT m_iCount;

		bool m_hasTex;
		bool m_hasNormalMap;
	};
}


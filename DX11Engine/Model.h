#pragma once

#include "stdafx.h"

namespace DX11Engine
{
	class Model
	{
	public:
		Model();
		~Model();

		bool Init(ID3D11Device* device, Vertex verticies[], UINT vCount, DWORD indicies[], UINT iCount);
		bool Draw(ID3D11Device* device, ID3D11DeviceContext* devcon);
		void Release();

		bool Loaded;

	private:
		ID3D11Buffer* m_verticies;
		ID3D11Buffer* m_indicies;

		UINT m_vCount;
		UINT m_iCount;
	};
}


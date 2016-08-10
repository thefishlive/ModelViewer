#pragma once

#include "stdafx.h"

namespace DX11Engine
{
	class Shader
	{
	public:
		Shader(LPCTSTR file, LPCSTR type, LPCSTR version);
		~Shader();

		void Release();

		virtual void LoadShader(ID3D11Device* device) = 0;
		virtual void BindShader(ID3D11DeviceContext* devcon) = 0;

		ID3D10Blob* Bytecode;
	};
}


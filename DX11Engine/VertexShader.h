#pragma once

#include "Shader.h"

namespace DX11Engine
{
	class VertexShader : public Shader
	{
	public:
		VertexShader(LPCTSTR file, LPCSTR main);
		~VertexShader();

		void LoadShader(ID3D11Device* device);
		void BindShader(ID3D11DeviceContext * devcon);

		bool Loaded;
	private:
		ID3D11VertexShader* m_shader;
	};
}


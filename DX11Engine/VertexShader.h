#pragma once

#include "Shader.h"

namespace DX11Engine
{
	class VertexShader : public Shader
	{
	public:
		VertexShader(LPCTSTR file, LPCSTR main);
		~VertexShader();

		bool LoadShader(ID3D11Device* device);
		bool BindShader(ID3D11DeviceContext * devcon);

	private:
		ID3D11VertexShader* m_shader;
	};
}


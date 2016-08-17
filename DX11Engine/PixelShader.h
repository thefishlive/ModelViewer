#pragma once
#include "Shader.h"
namespace DX11Engine
{
	class PixelShader : public Shader
	{
	public:
		PixelShader(LPCTSTR file, LPCSTR main);
		~PixelShader();

		bool LoadShader(ID3D11Device* device);
		bool BindShader(ID3D11DeviceContext * devcon);

	private:
		ID3D11PixelShader* m_shader;
	};
}


#pragma once
#include "Shader.h"
namespace DX11Engine
{
	class PixelShader : public Shader
	{
	public:
		PixelShader(LPCTSTR file, LPCSTR main);
		~PixelShader();

		void LoadShader(ID3D11Device* device);
		void BindShader(ID3D11DeviceContext * devcon);

		bool Loaded;
	private:
		ID3D11PixelShader* m_shader;
	};
}


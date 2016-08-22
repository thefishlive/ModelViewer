#include "PixelShader.h"

DX11Engine::PixelShader::PixelShader(LPCTSTR file, LPCSTR main) :
	Shader(file, main, PIXEL_SHADER_VERSION)
{
}

DX11Engine::PixelShader::~PixelShader()
{
}

bool DX11Engine::PixelShader::LoadShader(ID3D11Device * device)
{
	if (Errored)
		return false;

	HRESULT result;
	result = device->CreatePixelShader(Bytecode->GetBufferPointer(), Bytecode->GetBufferSize(), NULL, &m_shader);
	HR_B(result, TEXT("device->CreatePixelShader"));

	Loaded = true;
	return true;
}

bool DX11Engine::PixelShader::BindShader(ID3D11DeviceContext * devcon)
{
	if (!Loaded)
		return false;

	devcon->PSSetShader(m_shader, 0, 0);
	return true;
}

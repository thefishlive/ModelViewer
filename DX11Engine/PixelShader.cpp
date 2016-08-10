#include "PixelShader.h"

DX11Engine::PixelShader::PixelShader(LPCTSTR file, LPCSTR main) :
	Shader(file, main, PIXEL_SHADER_VERSION)
{
}

DX11Engine::PixelShader::~PixelShader()
{
}

void DX11Engine::PixelShader::LoadShader(ID3D11Device * device)
{
	HRESULT result;
	result = device->CreatePixelShader(Bytecode->GetBufferPointer(), Bytecode->GetBufferSize(), NULL, &m_shader);
	CHECK_RESULT_VOID(result, TEXT("device->CreatePixelShader"));

	Loaded = true;
}

void DX11Engine::PixelShader::BindShader(ID3D11DeviceContext * devcon)
{
	if (!Loaded)
		return;

	devcon->PSSetShader(m_shader, 0, 0);
}

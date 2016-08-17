#include "VertexShader.h"

DX11Engine::VertexShader::VertexShader(LPCTSTR file, LPCSTR main) :
	Shader(file, main, VERTEX_SHADER_VERSION)
{
}

DX11Engine::VertexShader::~VertexShader()
{
}

bool DX11Engine::VertexShader::LoadShader(ID3D11Device * device)
{
	if (Errored)
		return false;

	HRESULT result;
	result = device->CreateVertexShader(Bytecode->GetBufferPointer(), Bytecode->GetBufferSize(), NULL, &m_shader);
	CHECK_RESULT_BOOL(result, TEXT("device->CreateVertexShader"));

	Loaded = true;
	return true;
}

bool DX11Engine::VertexShader::BindShader(ID3D11DeviceContext* devcon)
{
	if (!Loaded)
		return false;

	devcon->VSSetShader(m_shader, 0, 0);
	return true;
}

#include "VertexShader.h"

DX11Engine::VertexShader::VertexShader(LPCTSTR file, LPCSTR main) :
	Shader(file, main, VERTEX_SHADER_VERSION)
{
}

DX11Engine::VertexShader::~VertexShader()
{
}

void DX11Engine::VertexShader::LoadShader(ID3D11Device * device)
{
	HRESULT result;
	result = device->CreateVertexShader(Bytecode->GetBufferPointer(), Bytecode->GetBufferSize(), NULL, &m_shader);
	CHECK_RESULT_VOID(result, TEXT("device->CreateVertexShader"));

	Loaded = true;
}

void DX11Engine::VertexShader::BindShader(ID3D11DeviceContext* devcon)
{
	if (!Loaded)
		return;

	devcon->VSSetShader(m_shader, 0, 0);
}

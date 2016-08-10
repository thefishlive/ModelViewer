#include "Shader.h"

DX11Engine::Shader::Shader(LPCTSTR file, LPCSTR type, LPCSTR version)
{
	HRESULT result;
	result = D3DX11CompileFromFile(file, 0, 0, type, version, 0, 0, 0, &Bytecode, 0, 0);
	CHECK_RESULT_VOID(result, TEXT("D3DX11CompileFromFile"));
}

DX11Engine::Shader::~Shader()
{
}

void DX11Engine::Shader::Release()
{
	SAFE_RELEASE(Bytecode);
}

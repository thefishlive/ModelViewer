#include "Shader.h"

#include <d3dcompiler.h>

DX11Engine::Shader::Shader(LPCTSTR file, LPCSTR entry, LPCSTR version)
{
	HRESULT result;
	UINT flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	result = D3DX11CompileFromFile(file, 0, ((ID3DInclude*)(UINT_PTR)1), entry, version, 0, flags, 0, &Bytecode, 0, 0);
	CHECK_RESULT_VOID(result, TEXT("D3DX11CompileFromFile"));
}

DX11Engine::Shader::~Shader()
{
}

void DX11Engine::Shader::Release()
{
	SAFE_RELEASE(Bytecode);
}

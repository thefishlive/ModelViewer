#include "Shader.h"

#include <d3dcompiler.h>

DX11Engine::Shader::Shader(LPCTSTR file, LPCSTR entry, LPCSTR version)
{
	HRESULT result;
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3D10Blob* error;

#if defined( DEBUG ) || defined ( _DEBUG )
	// Load from raw shader
	result = D3DX11CompileFromFile(file, 0, ((ID3DInclude*)(UINT_PTR)1), entry, version, 0, flags, 0, &Bytecode, &error, 0);
#else
	// Load from compiled shader
	// TODO load compiled shader
#endif

	Errored = FAILED(result);

#if defined( DEBUG ) || defined ( _DEBUG )
	if (error)
	{
		OutputErrorMessage(error, file, false);
		error->Release();
	}
#endif

	if (Errored)
	{
		if (error)
		{
			OutputErrorMessage(error, file);
			MessageBox(NULL, TEXT("D3DX11CompileFromFile"), TEXT("Direct 3D Error"), MB_ICONERROR | MB_OK);
			error->Release();
		}

		SAFE_RELEASE(Bytecode);
	}
}

DX11Engine::Shader::~Shader()
{
}

void DX11Engine::Shader::Release()
{
	SAFE_RELEASE(Bytecode);
}

void DX11Engine::Shader::OutputErrorMessage(ID3D10Blob * error, LPCTSTR file, bool show)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;
	wstring errorFile = std::wstring(file) + L".error";

	compileErrors = (char*)(error->GetBufferPointer());
	bufferSize = error->GetBufferSize();

	// Open a file to write the error message to.
	fout.open(errorFile);

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	SAFE_RELEASE(error);

	if (show)
	{
		MessageBox(0, L"Error compiling shader.  Check logs for message.", file, MB_OK);
	}
}

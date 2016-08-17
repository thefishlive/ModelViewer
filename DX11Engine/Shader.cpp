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

	result = D3DX11CompileFromFile(file, 0, ((ID3DInclude*)(UINT_PTR)1), entry, version, 0, flags, 0, &Bytecode, &error, 0);
	Errored = FAILED(result);

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

void DX11Engine::Shader::OutputErrorMessage(ID3D10Blob * error, LPCTSTR file)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(error->GetBufferPointer());

	// Get the length of the message.
	bufferSize = error->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	SAFE_RELEASE(error);

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(0, L"Error compiling shader.  Check shader-error.txt for message.", file, MB_OK);
}

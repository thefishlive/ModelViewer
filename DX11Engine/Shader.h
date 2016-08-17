#pragma once

#include "stdafx.h"

#include <fstream>

using namespace std;

namespace DX11Engine
{
	class Shader
	{
	public:
		Shader(LPCTSTR file, LPCSTR entry, LPCSTR version);
		~Shader();

		virtual bool LoadShader(ID3D11Device* device) = 0;
		virtual bool BindShader(ID3D11DeviceContext* devcon) = 0;

		void Release();

		ID3D10Blob* Bytecode;

		bool Loaded;
		bool Errored;

	private:
		void OutputErrorMessage(ID3D10Blob* error, LPCTSTR file);
	};
}


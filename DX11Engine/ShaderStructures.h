#pragma once

#include <xnamath.h>

namespace DX11Engine
{
	struct Vertex
	{
		Vertex() {}
		Vertex(float x, float y, float z,
			float u, float v,
			float nx, float ny, float nz)
			: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

		XMFLOAT3 pos;
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
	};

	struct Light
	{
		Light()
		{
			ZeroMemory(this, sizeof(Light));
		}

		XMFLOAT3 dir;
		float pad;
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
	};

	struct WVPBuffer
	{
		XMMATRIX WVP;
		XMMATRIX World;
	};

	struct LightBuffer
	{
		Light light;
	};

}
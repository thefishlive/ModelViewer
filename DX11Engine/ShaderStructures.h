#pragma once

#include <xnamath.h>

#define MAX_LIGHTS 3

namespace DX11Engine
{
	enum LightType
	{
		Unlit = 0,
		Direcitonal = 1,
		Point = 2,
		Spot = 3
	};

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

		LightType type;
		XMFLOAT3 position;

		XMFLOAT3 dir;
		float pad;

		float range;
		XMFLOAT3 att;

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
		Light light[3];
	};

}
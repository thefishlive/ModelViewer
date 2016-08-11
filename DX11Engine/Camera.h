#pragma once

#include "stdafx.h"

namespace DX11Engine
{
	class Camera
	{
	public:
		Camera(XMVECTOR position, XMVECTOR target, XMVECTOR up, float fov, float aspect, float near, float far);
		~Camera();

		CXMMATRIX BuildWVP(CXMMATRIX World);

		void Recalcuate();
		void Update();

		XMVECTOR Position;
		XMVECTOR Target;
		XMVECTOR Up;

		float Fov;
		float Aspect;
		float NearZ;
		float FarZ;
	private:
		XMMATRIX m_view;
		XMMATRIX m_projection;

	};
}


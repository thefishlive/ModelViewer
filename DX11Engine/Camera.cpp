#include "Camera.h"

DX11Engine::Camera::Camera(XMVECTOR position, XMVECTOR target, XMVECTOR up, float fov, float aspect, float nearZ, float farZ) :
	Position(position),
	Target(target),
	Up(up),
	Fov(fov),
	Aspect(aspect),
	NearZ(nearZ),
	FarZ(farZ)
{
	Recalcuate();
}

DX11Engine::Camera::~Camera()
{
}

CXMMATRIX DX11Engine::Camera::BuildWVP(CXMMATRIX World)
{
	return World * m_view * m_projection;
}

void DX11Engine::Camera::Update()
{
}

void DX11Engine::Camera::Recalcuate()
{
	m_view = XMMatrixLookAtLH(Position, Target, Up);
	m_projection = XMMatrixPerspectiveFovLH(Fov, Aspect, NearZ, FarZ);
}

#include "Model.h"

DX11Engine::Model::Model()
{
}

DX11Engine::Model::~Model()
{
}

bool DX11Engine::Model::Init(ID3D11Device* device, Vertex verticies[], UINT vCount, DWORD indicies[], UINT iCount)
{
	m_vCount = vCount;
	m_iCount = iCount;

	HRESULT result;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	// Create vertex buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(Vertex) * vCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = verticies;

	result = device->CreateBuffer(&desc, &data, &m_verticies);
	CHECK_RESULT_BOOL(result, TEXT("device->CreateBuffer"));

	// Create index buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(DWORD) * iCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = indicies;

	result = device->CreateBuffer(&desc, &data, &m_indicies);
	CHECK_RESULT_BOOL(result, TEXT("device->CreateBuffer"));

	Loaded = true;
	return true;
}

bool DX11Engine::Model::Draw(ID3D11Device* device, ID3D11DeviceContext* devcon)
{
	if (!Loaded)
		return false;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	devcon->IASetVertexBuffers(0, 1, &m_verticies, &stride, &offset);
	devcon->IASetIndexBuffer(m_indicies, DXGI_FORMAT_R32_UINT, 0);
	devcon->DrawIndexed(m_iCount, 0, 0);

	return true;
}

void DX11Engine::Model::Release()
{
	SAFE_RELEASE(m_verticies);
	SAFE_RELEASE(m_indicies);
}

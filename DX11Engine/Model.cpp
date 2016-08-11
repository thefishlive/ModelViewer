#include "Model.h"

DX11Engine::Model::Model()
{
	Transformation = XMMatrixIdentity();
}

DX11Engine::Model::~Model()
{
}

bool DX11Engine::Model::Init(ID3D11Device* device, Vertex verticies[], UINT vCount, DWORD indicies[], UINT iCount, LPCTSTR texturename)
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

	// Load texture
	if (texturename != L"")
	{
		m_hasTex = true;

		result = D3DX11CreateShaderResourceViewFromFile(device, texturename, NULL, NULL, &m_texture, NULL);
		CHECK_RESULT_BOOL(result, TEXT("D3DX11CreateShaderResourceViewFromFile"));

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		result = device->CreateSamplerState(&sampDesc, &m_sampler);
		CHECK_RESULT_BOOL(result, TEXT("device->CreateSamplerState"));
	}

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

	if (m_hasTex)
	{
		devcon->PSSetShaderResources(0, 1, &m_texture);
		devcon->PSSetSamplers(0, 1, &m_sampler);
	}

	devcon->DrawIndexed(m_iCount, 0, 0);

	return true;
}

void DX11Engine::Model::Release()
{
	SAFE_RELEASE(m_verticies);
	SAFE_RELEASE(m_indicies);
}

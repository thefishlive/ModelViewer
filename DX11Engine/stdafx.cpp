#include "stdafx.h"

void CreateBuffer(ID3D11Device* device, ID3D11Buffer **target, UINT flags, const void* data, UINT size)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = size;
	desc.BindFlags = flags;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT result;
	if (data != NULL)
	{
		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(D3D11_SUBRESOURCE_DATA));

		subData.pSysMem = data;
		result = device->CreateBuffer(&desc, &subData, target);
		CHECK_RESULT_VOID(result, TEXT("device->CreateBuffer"));
	}
	else 
	{
		result = device->CreateBuffer(&desc, NULL, target);
		CHECK_RESULT_VOID(result, TEXT("device->CreateBuffer"));
	}

}
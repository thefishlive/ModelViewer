#include "FontRenderer.h"



DX11Engine::FontRenderer::FontRenderer()
{
}


DX11Engine::FontRenderer::~FontRenderer()
{
}

bool DX11Engine::FontRenderer::Init(ID3D11Device * device, IDXGIAdapter1 * adapter, int width, int height)
{
	m_width = width;
	m_height = height;

	HRESULT result;

	// Setup Directx 10
	result = D3D10CreateDevice1(
		adapter,
		D3D10_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3,
		D3D10_1_SDK_VERSION,
		&m_device
	);
	CHECK_RESULT_BOOL(result, TEXT("D3D10CreateDevice1"));

	D3D11_TEXTURE2D_DESC sharedTexDesc;
	ZeroMemory(&sharedTexDesc, sizeof(D3D11_TEXTURE2D_DESC));

	sharedTexDesc.Width = width;
	sharedTexDesc.Height = height;
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	result = device->CreateTexture2D(&sharedTexDesc, NULL, &m_sharedTex);
	CHECK_RESULT_BOOL(result, TEXT("device->CreateTexture2D"));

	result = m_sharedTex->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_lock11);
	CHECK_RESULT_BOOL(result, TEXT("m_sharedTex->QueryInterface"));

	IDXGIResource* sharedResource10;
	HANDLE sharedHandle10;

	result = m_sharedTex->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);
	CHECK_RESULT_BOOL(result, TEXT("m_sharedTex->QueryInterface"));
	result = sharedResource10->GetSharedHandle(&sharedHandle10);
	CHECK_RESULT_BOOL(result, TEXT("sharedResource10->GetSharedHandle"));

	SAFE_RELEASE(sharedResource10);

	// Create shared surface
	IDXGISurface1* sharedSurface10;
	result = m_device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)&sharedSurface10);
	CHECK_RESULT_BOOL(result, TEXT("m_device->OpenSharedResource"));
	result = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_lock10);
	CHECK_RESULT_BOOL(result, TEXT("sharedSurface10->QueryInterface"));

	// Setup Direct2D
	ID2D1Factory *d2dFactory;
	result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&d2dFactory);
	CHECK_RESULT_BOOL(result, TEXT("D2D1CreateFactory"));

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProps;
	ZeroMemory(&renderTargetProps, sizeof(D2D1_RENDER_TARGET_PROPERTIES));

	renderTargetProps.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	result = d2dFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProps, &m_renderTarget);
	CHECK_RESULT_BOOL(result, TEXT("d2dFactory->CreateDxgiSurfaceRenderTarget"));

	SAFE_RELEASE(sharedSurface10);
	SAFE_RELEASE(d2dFactory);

	result = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &m_brush);
	CHECK_RESULT_BOOL(result, TEXT("m_renderTarget->CreateSolidColorBrush"));

	// Setup DirectWrite
	result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_dwriteFactory));
	CHECK_RESULT_BOOL(result, TEXT("DWriteCreateFactory"));

	result = m_dwriteFactory->CreateTextFormat(
		L"Script",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"en-us",
		&m_textFormat
	);
	CHECK_RESULT_BOOL(result, TEXT("m_dwriteFactory->CreateTextFormat"));

	result = m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	CHECK_RESULT_BOOL(result, TEXT("m_textFormat->SetTextAlignment"));
	result = m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	CHECK_RESULT_BOOL(result, TEXT("m_textFormat->SetParagraphAlignment"));

	m_device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	return true;
}

bool DX11Engine::FontRenderer::InitScreenTexture(ID3D11Device* device)
{
	HRESULT result;

	Vertex v[] =
	{
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f),
	};

	DWORD i[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	// Create Index buffer
	CreateBuffer(device, &m_indexBuffer, D3D11_BIND_INDEX_BUFFER, i, sizeof(DWORD) * _countof(i));
	CreateBuffer(device, &m_vertBuffer, D3D11_BIND_VERTEX_BUFFER, v, sizeof(Vertex) * _countof(v));
	CreateBuffer(device, &m_objectBuffer, D3D11_BIND_CONSTANT_BUFFER, NULL, sizeof(WVPBuffer));

	result = device->CreateShaderResourceView(m_sharedTex, NULL, &m_texture);
	CHECK_RESULT_BOOL(result, TEXT("device->CreateShaderResourceView"));

	// Create render state
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));

	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.MultisampleEnable = true;

	result = device->CreateRasterizerState(&rsDesc, &m_renderState);
	CHECK_RESULT_BOOL(result, TEXT("device->CreateRasterizerState"));

	// Create blend state
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	result = device->CreateBlendState(&blendDesc, &m_blendState);
	CHECK_RESULT_BOOL(result, TEXT("device->CreateBlendState"));

	// Create sampler
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

	// Create Light buffer
	CreateBuffer(device, &m_sceneBuffer, D3D11_BIND_CONSTANT_BUFFER, NULL, sizeof(LightBuffer));

	m_lightBuffer = LightBuffer();
	m_lightBuffer.light.type = LightType::Unlit;

	return true;
}

void DX11Engine::FontRenderer::PrintText(ID3D11DeviceContext* devcon, std::wstring text)
{
	// Aquire lock on shared surface
	m_lock11->ReleaseSync(0);
	m_lock10->AcquireSync(0, 5);

	m_renderTarget->BeginDraw();
	m_renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	// Build the string
	std::wostringstream printString;
	printString << text;
	std::wstring printText = printString.str();
	
	// Set the font color
	D2D1_COLOR_F fontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	m_brush->SetColor(fontColor);

	// Draw the text on screen
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, m_width, m_height);
	m_renderTarget->DrawTextW(printText.c_str(), wcslen(printText.c_str()), m_textFormat, layoutRect, m_brush);

	m_renderTarget->EndDraw();

	// Release lock on shared surface
	m_lock10->ReleaseSync(1);
	m_lock11->AcquireSync(1, 5);

	// Setup render state
	WVPBuffer wvpBuffer;
	wvpBuffer.World = XMMatrixIdentity();
	wvpBuffer.WVP = XMMatrixIdentity();

	// Setup Input Assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &m_vertBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Setup Vertex Shader
	devcon->UpdateSubresource(m_objectBuffer, 0, NULL, &wvpBuffer, 0, 0);
	devcon->VSSetConstantBuffers(0, 1, &m_objectBuffer);

	// Setup rasterizer stage
	devcon->RSSetState(m_renderState);

	// Setup Pixel Shader
	devcon->UpdateSubresource(m_sceneBuffer, 0, NULL, &m_lightBuffer, 0, 0);
	devcon->PSSetConstantBuffers(0, 1, &m_sceneBuffer);
	devcon->PSSetShaderResources(0, 1, &m_texture);
	devcon->PSSetSamplers(0, 1, &m_sampler);

	// Setup Output Merger
	devcon->OMSetBlendState(m_blendState, NULL, 0xffffffff);

	// Draw texture
	devcon->DrawIndexed(6, 0, 0);
}

void DX11Engine::FontRenderer::Release()
{
	SAFE_RELEASE(m_device);
	SAFE_RELEASE(m_lock11);
	SAFE_RELEASE(m_lock10);
	SAFE_RELEASE(m_renderTarget);
	SAFE_RELEASE(m_brush);
	SAFE_RELEASE(m_sharedTex);
	SAFE_RELEASE(m_vertBuffer);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_dwriteFactory);
	SAFE_RELEASE(m_textFormat);
}

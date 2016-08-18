#include "DirectXDevice.h"

using namespace DX11Engine;

DX11Engine::DirectXDevice::DirectXDevice(int width, int height, BOOL windowed, CCamera camera, D3DXCOLOR background) :
	Background(background),
	m_vs(VertexShader(L"VertexShader.hlsl", "main")),
	m_ps(PixelShader(L"StandardShader.hlsl", "main")),
	m_unlit(PixelShader(L"UnlitShader.hlsl", "main")),
	m_width(width),
	m_height(height),
	m_windowed(windowed),
	m_models(),
	Camera(camera),
	m_vsync(true)
{
}

DX11Engine::DirectXDevice::~DirectXDevice()
{
}

bool DX11Engine::DirectXDevice::InitDevice(HINSTANCE instance, HWND window)
{
	m_window = window;
	int width = m_width;
	int height = m_height;

	// Update width and height to fullscreen resolution
	if (!m_windowed)
	{
		HMONITOR hmon = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hmon, &mi);

		width = mi.rcMonitor.right - mi.rcMonitor.left;
		height = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}

	HRESULT result;

	// Create the DXGI adapter
	IDXGIFactory1* factory;
	result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
	CHECK_RESULT_BOOL(result, TEXT("CreateDXGIFactory1"));
	
	IDXGIAdapter1* adapter;
	result = factory->EnumAdapters1(0, &adapter);
	CHECK_RESULT_BOOL(result, TEXT("factory->EnumAdapters1"));
	SAFE_RELEASE(factory);

	// Create swap chain
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = width;
	bufferDesc.Height = height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SAMPLE_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(DXGI_SAMPLE_DESC));

	sampleDesc.Count = 4;
	sampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC scDesc;
	ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	scDesc.BufferDesc = bufferDesc;
	scDesc.SampleDesc = sampleDesc;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = 1;
	scDesc.OutputWindow = window;
	scDesc.Windowed = m_windowed;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	UINT flags =  D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	result = D3D11CreateDeviceAndSwapChain(
		adapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		flags,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scDesc,
		&m_swapChain,
		&m_device,
		NULL,
		&m_devcon
	);
	CHECK_RESULT_BOOL(result, TEXT("D3D11CreateDeviceAndSwapChain"));

	// Initialise D2D
	if (!m_fontRenderer.Init(m_device, adapter, width, height))
		return false;

	SAFE_RELEASE(adapter);

	// Create back buffer
	ID3D11Texture2D* backBuffer;
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backBuffer);
	CHECK_RESULT_BOOL(result, TEXT("m_swapChain->GetBuffer"));

	// Create render target view
	result = m_device->CreateRenderTargetView(backBuffer, NULL, &m_rtv);
	CHECK_RESULT_BOOL(result, TEXT("m_device->CreateRenderTargetView"));

	SAFE_RELEASE(backBuffer);

	// Setup depth stencil and buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc = sampleDesc;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthBuffer);
	m_device->CreateDepthStencilView(m_depthBuffer, NULL, &m_depthStencil);

	m_devcon->OMSetRenderTargets(1, &m_rtv, m_depthStencil);

	// Setup input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	result = m_device->CreateInputLayout(layout, ARRAYSIZE(layout), m_vs.Bytecode->GetBufferPointer(), m_vs.Bytecode->GetBufferSize(), &m_layout);
	CHECK_RESULT_BOOL(result, TEXT("m_device->CreateInputLayout"));
	m_devcon->IASetInputLayout(m_layout);
	m_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Setup viewport
	m_devcon->RSSetViewports(1, &(BuildViewport(width, height)));

	// Setup render state
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));

	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.MultisampleEnable = true;

	result = m_device->CreateRasterizerState(&rsDesc, &m_renderState);

	CHECK_RESULT_BOOL(result, TEXT("m_device->CreateRasterizerState"));
	m_devcon->RSSetState(m_renderState);

	return true;
}

bool DX11Engine::DirectXDevice::InitScene()
{
	HRESULT result;

	// Initialise D2D
	if (!m_fontRenderer.InitScreenTexture(m_device))
		return false;

	// Load Shaders
	if (!m_vs.LoadShader(m_device) || !m_vs.BindShader(m_devcon))
		return false;
	if (!m_ps.LoadShader(m_device) || !m_ps.BindShader(m_devcon))
		return false;
	if (!m_unlit.LoadShader(m_device))
		return false;

	// Load models
	// Load square
	Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,-1.0f, -1.0f, -1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,-1.0f,  1.0f, -1.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),

		// Back Face
		Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f,-1.0f, -1.0f, 1.0f),
		Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f),
		Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f),
		Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f,-1.0f,  1.0f, 1.0f),

		// Top Face
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,-1.0f, 1.0f, -1.0f),
		Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f,-1.0f, 1.0f,  1.0f),
		Vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f),
		Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f),

		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,-1.0f, -1.0f, -1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
		Vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, -1.0f,  1.0f),
		Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,-1.0f, -1.0f,  1.0f),

		// Left Face
		Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f,-1.0f, -1.0f,  1.0f),
		Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f,-1.0f,  1.0f,  1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f,-1.0f,  1.0f, -1.0f),
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,-1.0f, -1.0f, -1.0f),

		// Right Face
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
		Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  1.0f, -1.0f),
		Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  1.0f),
		Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f),
	};

	DWORD i[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	Model cube = Model();

	if (!cube.Init(m_device, v, _countof(v), i, _countof(i), L"AncientMayanBlocks-ColorMap.png"))
		return false;

	cube.Transformation = XMMatrixIdentity();
	m_models.push_back(cube);

	CreateBuffer(m_device, &m_wvpBuffer, D3D11_BIND_CONSTANT_BUFFER, NULL, sizeof(WVPBuffer));
	CreateBuffer(m_device, &m_sceneBuffer, D3D11_BIND_CONSTANT_BUFFER, NULL, sizeof(LightBuffer));

	// Setup lights
	Light light1 = Light();
	light1.type = LightType::Point;
	light1.position = XMFLOAT3(0.0f, 2.5f, 0.0f);
	light1.att = XMFLOAT3(0.0f, 0.2f, 0.0f);
	light1.range = 100.0f;
	light1.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light1.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_lightBuffer.light[0] = light1;

	Light light2 = Light();
	light2.type = LightType::Point;
	light2.position = XMFLOAT3(2.5f, 0.0f, 0.0f);
	light2.att = XMFLOAT3(0.0f, 0.2f, 0.0f);
	light2.range = 100.0f;
	light2.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light2.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_lightBuffer.light[1] = light2;

	Light light3 = Light();
	light3.type = LightType::Point;
	light3.position = XMFLOAT3(0.0f, 0.0f, 2.5f);
	light3.att = XMFLOAT3(0.0f, 0.2f, 0.0f);
	light3.range = 100.0f;
	light3.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light3.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_lightBuffer.light[2] = light3;

	return true;
}

bool DX11Engine::DirectXDevice::UpdateScene(float time)
{
	return true;
}

bool DX11Engine::DirectXDevice::DrawScene(FPSTimer timer)
{
	m_devcon->ClearRenderTargetView(m_rtv, Background);
	m_devcon->ClearDepthStencilView(m_depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_devcon->UpdateSubresource(m_sceneBuffer, 0, NULL, &m_lightBuffer, 0, 0);
	m_devcon->PSSetConstantBuffers(0, 1, &m_sceneBuffer);

	if (!m_ps.BindShader(m_devcon))
		return false;

	WVPBuffer wvpBuffer;
	XMMATRIX wvp;

	for (auto &i : m_models) 
	{
		Camera.Recalcuate();
		wvp = Camera.BuildWVP(i.Transformation);
		wvpBuffer.WVP = XMMatrixTranspose(wvp);
		wvpBuffer.World = XMMatrixIdentity();

		m_devcon->UpdateSubresource(m_wvpBuffer, 0, NULL, &wvpBuffer, 0, 0);
		m_devcon->VSSetConstantBuffers(0, 1, &m_wvpBuffer);

		i.Draw(m_devcon);
	}

	if (!m_unlit.BindShader(m_devcon))
		return false;

	m_fontRenderer.PrintText(m_devcon, L"FPS: " + std::to_wstring(timer.FPS) + L"   Frame Time: " + std::to_wstring(timer.FrameTime) + L"s");

	m_swapChain->Present(m_vsync ? 1 : 0, 0);
	return true;
}

void DX11Engine::DirectXDevice::SetFullscreen(bool fullscreen)
{
	int width = m_width;
	int height = m_height;

	// Update width and height to fullscreen resolution
	if (fullscreen)
	{
		HMONITOR hmon = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hmon, &mi);

		width = mi.rcMonitor.right - mi.rcMonitor.left;
		height = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}

	// Set fullscreen
	m_windowed = !fullscreen;
	m_swapChain->SetFullscreenState(fullscreen, NULL);

	// Update swapchain
	m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// Update viewport
	m_devcon->RSSetViewports(1, &(BuildViewport(width, height)));
}

bool DX11Engine::DirectXDevice::Release()
{
	m_swapChain->SetFullscreenState(FALSE, NULL);

	SAFE_RELEASE(m_swapChain);
	SAFE_RELEASE(m_device);
	SAFE_RELEASE(m_devcon);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_depthStencil);
	SAFE_RELEASE(m_depthBuffer);
	SAFE_RELEASE(m_wvpBuffer);
	SAFE_RELEASE(m_renderState);

	m_vs.Release();
	m_ps.Release();

	m_model.Release();
	m_fontRenderer.Release();

	return true;
}

D3D11_VIEWPORT DX11Engine::DirectXDevice::BuildViewport(int width, int height)
{
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	return viewport;
}

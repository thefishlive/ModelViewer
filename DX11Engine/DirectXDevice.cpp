#include "DirectXDevice.h"

using namespace DX11Engine;

DX11Engine::DirectXDevice::DirectXDevice(int width, int height, bool windowed, D3DXCOLOR background = { 0.0f, 0.0f, 0.0f, 1.0f }) :
	m_background(background),
	m_vs(VertexShader(L"Effect.fx", "VS")),
	m_ps(PixelShader(L"Effect.fx", "PS")),
	m_width(width),
	m_height(height),
	m_windowed(windowed),
	m_models()
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

	// Create swap chain
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = width;
	bufferDesc.Height = height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SAMPLE_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(DXGI_SAMPLE_DESC));

	sampleDesc.Count = 1;
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

	result = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	result = m_device->CreateInputLayout(layout, ARRAYSIZE(layout), m_vs.Bytecode->GetBufferPointer(), m_vs.Bytecode->GetBufferSize(), &m_layout);
	CHECK_RESULT_BOOL(result, TEXT("m_device->CreateInputLayout"));
	m_devcon->IASetInputLayout(m_layout);
	m_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Setup viewport
	m_devcon->RSSetViewports(1, &(BuildViewport(width, height)));

	return true;
}

bool DX11Engine::DirectXDevice::InitScene()
{
	HRESULT result;

	// Load Shaders
	m_vs.LoadShader(m_device);
	m_vs.BindShader(m_devcon);

	m_ps.LoadShader(m_device);
	m_ps.BindShader(m_devcon);

	// Load models
	Vertex v[] = 
	{
		Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f),
		Vertex(-0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f),
		Vertex(0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f),
		Vertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f),
	};

	DWORD i[] = 
	{
		0, 1, 2,
		0, 2, 3,
	};

	Model square = Model();

	if (!square.Init(m_device, v, _countof(v), i, _countof(i)))
		return false;

	m_models.push_back(square);

	Vertex triVerts[] =
	{
		Vertex(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f),
		Vertex(1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f),
		Vertex(0.5f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f),
	};

	DWORD triIndicies[] =
	{
		0, 2, 1
	};

	Model triangle = Model();

	if (!triangle.Init(m_device, triVerts, _countof(triVerts), triIndicies, _countof(triIndicies)))
		return false;

	m_models.push_back(triangle);
	return true;
}

bool DX11Engine::DirectXDevice::UpdateScene()
{
	return true;
}

bool DX11Engine::DirectXDevice::DrawScene()
{
	m_devcon->ClearRenderTargetView(m_rtv, m_background);
	m_devcon->ClearDepthStencilView(m_depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (auto &i : m_models) {
		i.Draw(m_device, m_devcon);
	}

	m_swapChain->Present(0, 0);
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

	m_vs.Release();
	m_ps.Release();

	m_model.Release();

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

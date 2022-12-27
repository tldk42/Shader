#include "D3D.h"

D3D::D3D(bool vSync)
	: mVsyncEnabled(vSync),
	  mVideoCardMemory(0),
	  mVideoCardDescription{},
	  mD3D(nullptr),
	  mD3DDevice(nullptr),
	  mSwapChain(nullptr),
	  mRenderTargetView(nullptr),
	  mDepthStencilBuffer(nullptr),
	  mDepthStencilState(nullptr),
	  mDepthStencilView(nullptr),
	  mRasterState(nullptr),
	  mProjectionMatrix(),
	  mWorldMatrix(),
	  mOrthoMatrix(),
	  mFeatureLevel()
{
}

bool D3D::Initialize(HWND hWnd, const float winWidth, const float winHeight, bool fullScreen, float screenDepth,
                     float screenNear)
{
#pragma region initialize variables
	/** for User Computer Description */
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	DXGI_MODE_DESC* displayModeList;
	unsigned int numModes, i, numerator{}, denominator{};
	size_t stringLength;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;
#pragma endregion initialize variables

#pragma region 그래픽카드, 모니터 주사율 정보 가져오기

	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory))))
	{
		return false;
	}

	if (FAILED(factory->EnumAdapters(0, &adapter)))
	{
		return false;
	}

	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
	{
		return false;
	}

	if (FAILED(
		adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
	{
		return false;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes,
		displayModeList)))
	{
		return false;
	}

	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)winWidth && displayModeList[i].Height == (unsigned int)winHeight)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	if (FAILED(adapter->GetDesc(&adapterDesc)))
	{
		return false;
	}

	mVideoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	error = wcstombs_s(&stringLength, mVideoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

#pragma endregion 그래픽카드, 모니터 주사율 정보 가져오기

#pragma region 메모리 해제

	delete[] displayModeList;
	displayModeList = nullptr;

	adapterOutput->Release();
	adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;

	factory->Release();
	factory = nullptr;

#pragma endregion 메모리 해제

#pragma region swapChain Description

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;

	swapChainDesc.BufferDesc.Width = winWidth;
	swapChainDesc.BufferDesc.Height = winHeight;

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	swapChainDesc.BufferDesc.RefreshRate.Numerator = mVsyncEnabled ? numerator : 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = mVsyncEnabled ? denominator : 1;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapChainDesc.OutputWindow = hWnd;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.Windowed = fullScreen ? false : true;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = 0;

#pragma endregion swapChain Description

#pragma region Direct3D Device, DeviceContext 초기화

	mFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &mFeatureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &mD3D, NULL, &mD3DDevice)))
	{
		return false;
	}

#pragma endregion Direct3D Device, DeviceContext 초기화

#pragma region BackBuffer Pointer to SwapChain
	if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
	{
		return false;
	}

	if (FAILED(mD3D->CreateRenderTargetView(backBufferPtr, NULL, &mRenderTargetView)))
	{
		return false;
	}

	backBufferPtr->Release();
	backBufferPtr = nullptr;
#pragma endregion BackBuffer Pointer to SwapChain

#pragma region Depth(Z)Buffer Description

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = winWidth;
	depthBufferDesc.Height = winHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

#pragma endregion Depth(Z)Buffer Description

#pragma region Depth Stencil Buffer Description

	if (FAILED(mD3D->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer)))
	{
		return false;
	}

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(mD3D->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState)))
	{
		return false;
	}

	mD3DDevice->OMSetDepthStencilState(mDepthStencilState, 1);

#pragma endregion Depth Stencil Buffer Description

#pragma region Depth Stencil State Description

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(mD3D->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView)))
	{
		return false;
	}

#pragma endregion Depth Stencil State Description

	mD3DDevice->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

#pragma region Resterizer Description

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(mD3D->CreateRasterizerState(&rasterDesc, &mRasterState)))
	{
		return false;
	}

	mD3DDevice->RSSetState(mRasterState);

#pragma endregion Resterizer Description

#pragma region Viewport Setting for Rendering

	viewport.Width = static_cast<float>(winWidth);
	viewport.Height = static_cast<float>(winHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	mD3DDevice->RSSetViewports(1, &viewport);

#pragma endregion Viewport Setting for Rendering

	fieldOfView = static_cast<float>(DirectX::XM_PI) / 4.0f;
	screenAspect = static_cast<float>(winWidth) / static_cast<float>(winHeight);

	mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	mWorldMatrix = DirectX::XMMatrixIdentity();

	mOrthoMatrix = DirectX::XMMatrixOrthographicLH(winWidth, winHeight, screenNear, screenDepth);

	return true;
	// 	unsigned numerator;
	// 	unsigned denominator;
	// 	mVsyncEnabled = vSyncEnabled;
	//
	// #pragma region Swap Chain 초기화
	//
	// 	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	// 	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	// 	swapChainDesc.BufferCount = 1;
	// 	swapChainDesc.BufferDesc.Width = winWidth;
	// 	swapChainDesc.BufferDesc.Height = winHeight;
	// 	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	// 	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	// 	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// 	swapChainDesc.OutputWindow = hWnd;
	// 	swapChainDesc.SampleDesc.Count = 1;
	// 	swapChainDesc.SampleDesc.Quality = 0;
	// 	swapChainDesc.Windowed = TRUE;
	// 	swapChainDesc.Windowed = fullScreen ? false : true;
	// 	//swapChainDesc.BufferDesc.RefreshRate.Numerator = 
	//
	// #pragma endregion Swap Chain 초기화
	//
	// #pragma region Device & Swap Chain 초기화
	// 	if (FAILED(D3D11CreateDevice(nullptr,
	// 		D3D_DRIVER_TYPE_HARDWARE,
	// 		nullptr,
	// 		0,
	// 		nullptr,
	// 		0,
	// 		D3D11_SDK_VERSION,
	// 		&mD3D,
	// 		&mFeatureLevel,
	// 		&mD3DDevice)))
	// 	{
	// 		return false;
	// 	}
	// 	mSwapChain = CreateSwapChain(mD3D, &swapChainDesc);
	//
	//
	// #pragma endregion Device & Swap Chain 초기화
	//
	// #pragma region RTV(Rendering Target View) 초기화
	//
	// 	ID3D11Texture2D* backBuffer = nullptr;
	// 	if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer)))
	// 	{
	// 		return false;
	// 	}
	// 	if (FAILED(mD3D->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView)))
	// 	{
	// 		return false;
	// 	}
	// 	backBuffer->Release();
	//
	// #pragma endregion RTV(Rendering Target View) 초기화
	//
	// 	//mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH()
	//
	// #pragma region View Port 초기화
	//
	// 	D3D11_VIEWPORT viewport;
	// 	viewport.Width = winWidth;
	// 	viewport.Height = winHeight;
	// 	viewport.MinDepth = 0.f;
	// 	viewport.MaxDepth = 1.f;
	// 	viewport.TopLeftX = 0;
	// 	viewport.TopLeftY = 0;
	// 	mD3DDevice->RSSetViewports(1, &viewport);
	//
	// #pragma endregion View Port 초기화
	//
	// 	return true;
}

void D3D::Clear()
{
	if (mSwapChain)
	{
		mSwapChain->SetFullscreenState(false, nullptr);
		mSwapChain = nullptr;
	}

	if (mRasterState)
	{
		mRasterState->Release();
		mRasterState = nullptr;
	}

	if (mDepthStencilView)
	{
		mDepthStencilView->Release();
		mDepthStencilView = nullptr;
	}

	if (mDepthStencilState)
	{
		mDepthStencilState->Release();
		mDepthStencilState = nullptr;
	}

	if (mDepthStencilBuffer)
	{
		mDepthStencilBuffer->Release();
		mDepthStencilBuffer = nullptr;
	}

	if (mRenderTargetView)
	{
		mRenderTargetView->Release();
		mRenderTargetView = nullptr;
	}

	if (mD3DDevice)
	{
		mD3DDevice->Release();
		mD3DDevice = nullptr;
	}

	if (mD3D)
	{
		mD3D->Release();
		mD3D = nullptr;
	}

	if (mSwapChain)
	{
		mSwapChain->Release();
		mSwapChain = nullptr;
	}
}

void D3D::BeginScene(float red, float green, float blue, float alpha)
{
	const float color[4]{red, green, blue, alpha};

	mD3DDevice->ClearRenderTargetView(mRenderTargetView, color);

	mD3DDevice->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::EndScene()
{
	mSwapChain->Present(mVsyncEnabled ? 1 : 0, 0);
}

// IDXGISwapChain* D3D::CreateSwapChain(ID3D11Device* device, DXGI_SWAP_CHAIN_DESC* sd)
// {
// 	IDXGIDevice* dxGiDevice = nullptr;
// 	IDXGIAdapter* dxGiAdapter = nullptr;
// 	IDXGIFactory* dxGiFactory = nullptr;
// 	IDXGISwapChain* swapChain = nullptr;
//
// 	device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxGiDevice);
// 	dxGiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxGiAdapter);
// 	dxGiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxGiFactory);
// 	dxGiFactory->CreateSwapChain(device, sd, &swapChain);
//
// 	return swapChain;
// }

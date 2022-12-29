#include "D3D.h"
#include "Log.h"

D3D::D3D(const bool vSync)
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
	/** for User Computer Description
	 * I로 시작하는 인터페이스는 Com개체 이므로 reference count에 포함
	 * Release 해줘야함
	 */
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;

	DXGI_MODE_DESC* displayModeList;
	unsigned int numModes, i, numerator{}, denominator{};
	size_t stringLength;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;

	D3D11_RASTERIZER_DESC rasterDesc;
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

	JC_CORE_INFO(mVideoCardDescription);
	JC_CORE_INFO(mVideoCardMemory);

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

	ZeroMemory(&mSwapChainDesc, sizeof(mSwapChainDesc));

	mSwapChainDesc.BufferCount = 1;
	mSwapChainDesc.BufferDesc.Width = winWidth;
	mSwapChainDesc.BufferDesc.Height = winHeight;
	mSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mSwapChainDesc.BufferDesc.RefreshRate.Numerator = mVsyncEnabled ? numerator : 0;
	mSwapChainDesc.BufferDesc.RefreshRate.Denominator = mVsyncEnabled ? denominator : 1;
	mSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	mSwapChainDesc.OutputWindow = hWnd;
	mSwapChainDesc.SampleDesc.Count = 1;
	mSwapChainDesc.SampleDesc.Quality = 0;
	mSwapChainDesc.Windowed = fullScreen ? false : true;
	mSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	mSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	mSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	mSwapChainDesc.Flags = 0;

#pragma endregion swapChain Description

#pragma region Direct3D Device, DeviceContext 초기화

	mFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &mFeatureLevel, 1,
		D3D11_SDK_VERSION, &mSwapChainDesc, &mSwapChain, &mD3D, NULL, &mD3DDevice)))
	{
		return false;
	}

#pragma endregion Direct3D Device, DeviceContext 초기화

#pragma region BackBuffer Pointer to SwapChain

	ID3D11Texture2D* backBufferPtr;

	if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBufferPtr))))
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

	ZeroMemory(&mDepthBufferDesc, sizeof(mDepthBufferDesc));

	mDepthBufferDesc.Width = winWidth;
	mDepthBufferDesc.Height = winHeight;
	mDepthBufferDesc.MipLevels = 1;
	mDepthBufferDesc.ArraySize = 1;
	mDepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	mDepthBufferDesc.SampleDesc.Count = 1;
	mDepthBufferDesc.SampleDesc.Quality = 0;
	mDepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	mDepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	mDepthBufferDesc.CPUAccessFlags = 0;
	mDepthBufferDesc.MiscFlags = 0;

#pragma endregion Depth(Z)Buffer Description

#pragma region Depth Stencil Buffer Description

	if (FAILED(mD3D->CreateTexture2D(&mDepthBufferDesc, NULL, &mDepthStencilBuffer)))
	{
		return false;
	}

	ZeroMemory(&mDepthStencilDesc, sizeof(mDepthStencilDesc));

	mDepthStencilDesc.DepthEnable = true;
	mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	mDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	mDepthStencilDesc.StencilEnable = true;
	mDepthStencilDesc.StencilReadMask = 0xFF;
	mDepthStencilDesc.StencilWriteMask = 0xFF;
	mDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	mDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	mDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	mDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(mD3D->CreateDepthStencilState(&mDepthStencilDesc, &mDepthStencilState)))
	{
		return false;
	}

	mD3DDevice->OMSetDepthStencilState(mDepthStencilState, 1);

#pragma endregion Depth Stencil Buffer Description

#pragma region Depth Stencil State Description

	ZeroMemory(&mDepthStencilViewDesc, sizeof(mDepthStencilViewDesc));

	mDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	mDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	mDepthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(mD3D->CreateDepthStencilView(mDepthStencilBuffer, &mDepthStencilViewDesc, &mDepthStencilView)))
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

	mViewport.Width = static_cast<float>(winWidth);
	mViewport.Height = static_cast<float>(winHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;

	mD3DDevice->RSSetViewports(1, &mViewport);

#pragma endregion Viewport Setting for Rendering

	mFOV = static_cast<float>(DirectX::XM_PI) / 4.0f;
	mScreenAspect = static_cast<float>(winWidth) / static_cast<float>(winHeight);

	mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(mFOV, mScreenAspect, screenNear, screenDepth);
	mWorldMatrix = DirectX::XMMatrixIdentity();
	mOrthoMatrix = DirectX::XMMatrixOrthographicLH(winWidth, winHeight, screenNear, screenDepth);

	return true;
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

HRESULT D3D::ResizeSwapChain(const UINT width, const UINT height)
{
	ID3D11Texture2D* backBufferPtr;

	mD3DDevice->OMSetRenderTargets(0, nullptr, nullptr);

	if (mRenderTargetView != nullptr)
	{
		mRenderTargetView->Release();
		mRenderTargetView = nullptr;
	}

	if (mDepthStencilView != nullptr)
	{
		mDepthStencilView->Release();
		mDepthStencilView = nullptr;
	}

	if (mDepthStencilState != nullptr)
	{
		mDepthStencilState->Release();
		mDepthStencilState = nullptr;
	}

	if (mDepthStencilBuffer != nullptr)
	{
		mDepthStencilBuffer->Release();
		mDepthStencilBuffer = nullptr;
	}

	if (FAILED(mSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
	{
		return S_FALSE;
	}

	if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBufferPtr))))
	{
		return S_FALSE;
	}

	if (FAILED(mD3D->CreateRenderTargetView(backBufferPtr, NULL, &mRenderTargetView)))
	{
		return S_FALSE;
	}

	mDepthBufferDesc.Width = width;
	mDepthBufferDesc.Height = height;

	if (FAILED(mD3D->CreateTexture2D(&mDepthBufferDesc, NULL, &mDepthStencilBuffer)))
	{
		return S_FALSE;
	}

	if (FAILED(mD3D->CreateDepthStencilState(&mDepthStencilDesc, &mDepthStencilState)))
	{
		return S_FALSE;
	}

	mD3DDevice->OMSetDepthStencilState(mDepthStencilState, 1);

	if (FAILED(mD3D->CreateDepthStencilView(mDepthStencilBuffer, &mDepthStencilViewDesc, &mDepthStencilView)))
	{
		return S_FALSE;
	}

	mD3DDevice->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	backBufferPtr->Release();
	backBufferPtr = nullptr;

	mViewport.Width = static_cast<float>(width);
	mViewport.Height = static_cast<float>(height);

	mD3DDevice->RSSetViewports(1, &mViewport);

	mScreenAspect = static_cast<float>(width) / static_cast<float>(height);
	
	mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(mFOV, mScreenAspect, 0.1f, 1000.f);
	mWorldMatrix = DirectX::XMMatrixIdentity();
	
	mOrthoMatrix = DirectX::XMMatrixOrthographicLH(width, height, 0.1f, 1000.f);

	return true;
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

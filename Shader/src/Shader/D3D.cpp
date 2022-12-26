#include "D3D.h"

D3D::D3D()
	: mD3D(nullptr),
	  mD3DDevice(nullptr),
	  mSwapChain(nullptr),
	  mRenderTargetView(nullptr),
	  mFeatureLevel(),
	  mDriverType(D3D_DRIVER_TYPE_SOFTWARE)
{
}

bool D3D::Initialize(HWND hWnd, const float winWidth, const float winHeight, bool fullScreen, bool vSyncEnabled)
{

	unsigned numerator;
	unsigned denominator;
	mVsyncEnabled = vSyncEnabled;

#pragma region Swap Chain 초기화

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = winWidth;
	swapChainDesc.BufferDesc.Height = winHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Windowed = fullScreen ? false : true;
	//swapChainDesc.BufferDesc.RefreshRate.Numerator = 

#pragma endregion Swap Chain 초기화

#pragma region Device & Swap Chain 초기화
	if (FAILED(D3D11CreateDevice(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&mD3D,
		&mFeatureLevel,
		&mD3DDevice)))
	{
		return false;
	}
	mSwapChain = CreateSwapChain(mD3D, &swapChainDesc);


#pragma endregion Device & Swap Chain 초기화

#pragma region RTV(Rendering Target View) 초기화

	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer)))
	{
		return false;
	}
	if (FAILED(mD3D->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView)))
	{
		return false;
	}
	backBuffer->Release();

#pragma endregion RTV(Rendering Target View) 초기화

#pragma region View Port 초기화

	D3D11_VIEWPORT viewport;
	viewport.Width = winWidth;
	viewport.Height = winHeight;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	mD3DDevice->RSSetViewports(1, &viewport);

#pragma endregion View Port 초기화

	return true;
}

void D3D::Clear()
{
	if (mD3D)
	{
		mD3D->Release();
		mD3D = nullptr;
	}

	if (mD3DDevice)
	{
		mD3DDevice->Release();
		mD3DDevice = nullptr;
	}

	if (mSwapChain)
	{
		mSwapChain->Release();
		mSwapChain = nullptr;
	}

	if (mRenderTargetView)
	{
		mRenderTargetView->Release();
		mRenderTargetView = nullptr;
	}
}

IDXGISwapChain* D3D::CreateSwapChain(ID3D11Device* device, DXGI_SWAP_CHAIN_DESC* sd)
{
	IDXGIDevice* dxGiDevice = nullptr;
	IDXGIAdapter* dxGiAdapter = nullptr;
	IDXGIFactory* dxGiFactory = nullptr;
	IDXGISwapChain* swapChain = nullptr;

	device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxGiDevice);
	dxGiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxGiAdapter);
	dxGiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxGiFactory);
	dxGiFactory->CreateSwapChain(device, sd, &swapChain);

	return swapChain;
}

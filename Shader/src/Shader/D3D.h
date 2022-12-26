#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>

class D3D
{
public:
	D3D();
	D3D(const D3D&) = default;
	~D3D() = default;

	bool Initialize(HWND, float, float, bool, bool);
	void Clear();

	inline ID3D11DeviceContext* GetDevice() const;
	inline IDXGISwapChain* GetSwapChain() const;
	inline ID3D11RenderTargetView* GetRTV() const;

private:
	IDXGISwapChain* CreateSwapChain(ID3D11Device* device, DXGI_SWAP_CHAIN_DESC* sd);

private:
	bool mVsyncEnabled;
	ID3D11Device* mD3D;
	ID3D11DeviceContext* mD3DDevice;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mRenderTargetView;
	D3D_FEATURE_LEVEL mFeatureLevel;
	D3D_DRIVER_TYPE mDriverType;

};

 ID3D11DeviceContext* D3D::GetDevice() const
{
	return mD3DDevice;
}

 inline IDXGISwapChain* D3D::GetSwapChain() const
 {
	 return mSwapChain;
 }


 ID3D11RenderTargetView* D3D::GetRTV() const
 {
	 return mRenderTargetView;
 }



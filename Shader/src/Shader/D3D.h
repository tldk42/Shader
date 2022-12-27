#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class D3D
{
public:
	D3D(bool vSync);
	D3D() = delete;
	D3D(const D3D&) = default;
	~D3D() = default;

	bool Initialize(HWND, float, float, bool, float, float);
	void Clear();

	inline ID3D11Device* GetDevice() const;
	inline ID3D11DeviceContext* GetDeviceContext() const;
	inline IDXGISwapChain* GetSwapChain() const;
	inline ID3D11RenderTargetView* GetRTV() const;

	inline void GetProjectionMatrix(DirectX::XMMATRIX& mat) const
	{
		mat = mProjectionMatrix;
	}

	inline void GetWorldMatrix(DirectX::XMMATRIX& mat) const
	{
		mat = mWorldMatrix;
	}

	inline void GetOrthoMatrix(DirectX::XMMATRIX& mat) const
	{
		mat = mOrthoMatrix;
	}

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

// private:
// 	IDXGISwapChain* CreateSwapChain(ID3D11Device* device, DXGI_SWAP_CHAIN_DESC* sd);

private:
	bool mVsyncEnabled;
	int mVideoCardMemory;
	char mVideoCardDescription[128];

	ID3D11Device* mD3D;
	ID3D11DeviceContext* mD3DDevice;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11DepthStencilState* mDepthStencilState;
	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11RasterizerState* mRasterState;
	DirectX::XMMATRIX mProjectionMatrix;
	DirectX::XMMATRIX mWorldMatrix;
	DirectX::XMMATRIX mOrthoMatrix;
	D3D_FEATURE_LEVEL mFeatureLevel;
};

 ID3D11Device* D3D::GetDevice() const
{
	return mD3D;
}

 ID3D11DeviceContext* D3D::GetDeviceContext() const
 {
	 return mD3DDevice;
 }

 IDXGISwapChain* D3D::GetSwapChain() const
 {
	 return mSwapChain;
 }

 ID3D11RenderTargetView* D3D::GetRTV() const
 {
	 return mRenderTargetView;
 }

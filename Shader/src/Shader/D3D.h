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
	HRESULT ResizeSwapChain(UINT width, UINT height);

	inline ID3D11Device* GetDevice() const;
	inline ID3D11DeviceContext* GetDeviceContext() const;
	inline IDXGISwapChain* GetSwapChain() const;
	inline ID3D11RenderTargetView* GetRTV();
	inline const char* GetGraphicCardInfo() const;


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

	DXGI_SWAP_CHAIN_DESC mSwapChainDesc;
	D3D11_TEXTURE2D_DESC mDepthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC mDepthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC mDepthStencilViewDesc;

	DirectX::XMMATRIX mProjectionMatrix;
	DirectX::XMMATRIX mWorldMatrix;
	DirectX::XMMATRIX mOrthoMatrix;
	D3D_FEATURE_LEVEL mFeatureLevel;

	D3D11_VIEWPORT mViewport;

	float mFOV;
	float mScreenAspect;
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

 ID3D11RenderTargetView* D3D::GetRTV()
 {
	 return mRenderTargetView;
 }

inline const char* D3D::GetGraphicCardInfo() const
{
	return mVideoCardDescription;
}


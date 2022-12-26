#include "Graphic.h"

Graphic::Graphic()
	: mD3D(nullptr)
{
}

bool Graphic::Initialize(float width, float height, HWND hwnd)
{
	mD3D = new D3D;
	if (!mD3D)
	{
		return false;
	}

	if (!mD3D->Initialize(hwnd, width, height, FULL_SCREEN, VSYNC_ENABLED))
	{
		MessageBox(hwnd, "Could not Initialize Direct3D", "Error", MB_OK);
		return false;
	}
	return true;
}

void Graphic::Clear()
{
	if (mD3D)
	{
		mD3D->Clear();
	}
}

void Graphic::RenderFrame()
{
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	mD3D->GetDevice()->ClearRenderTargetView(mD3D->GetRTV(), ClearColor);
	mD3D->GetSwapChain()->Present(0, 0);
}

void Graphic::RenderScene()
{
}

void Graphic::RenderInfo()
{
}

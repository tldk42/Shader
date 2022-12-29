#pragma once

#include "D3D.h"

static bool FULL_SCREEN = false;
static bool VSYNC_ENABLED = true;
static float SCREEN_DEPTH = 1000.0f;
static float SCREEN_NEAR = 0.1f;

class Graphic
{
public:
	Graphic();
	Graphic(const Graphic&) = default;
	~Graphic() = default;

	bool Initialize(float, float, HWND);
	void Clear();
	
	bool RenderFrame();

	inline const char* GetGraphicCardDesc() const
	{
		return mD3D->GetGraphicCardInfo();
	}
	inline D3D* GetD3D() const
{
		return mD3D;
}

private:
	void RenderScene();
	void RenderInfo();
private:
	D3D* mD3D;
	class Camera* mCamera;
	class Model* mModel;
	class ColorShader* mColorShader;
};


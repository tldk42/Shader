#pragma once

#include "D3D.h"

static bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;

class Graphic
{
public:
	Graphic();
	Graphic(const Graphic&) = default;
	~Graphic() = default;

	bool Initialize(float, float, HWND);
	void Clear();

	void RenderFrame();

private:
	void RenderScene();
	void RenderInfo();
private:
	D3D* mD3D;
};


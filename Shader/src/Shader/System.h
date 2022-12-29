#pragma once

#include "framework.h"

static const char* graphicCard;

class System
{
public:
	System();
	~System() = default;

	bool Initialize();
	void Exit();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindow(int& width, int& height);
	void ExitWindow();


private:
	static const char* APPNAME;
	HINSTANCE mHinstance;
	HWND mHwnd;

	class ImGuiManager* mImGui;
	class Input* mInput;
	class Graphic* mGraphic;
};

static LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static System* gApplicationHandle = nullptr;
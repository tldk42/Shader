#pragma once

#include <Windows.h>

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

	class Input* mInput;
};

static LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static System* gApplicationHandle = nullptr;
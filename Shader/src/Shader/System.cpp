#include "System.h"
#include "Input.h"
#include "Graphic.h"
#include "Resource.h"
#include "ImGuiManager.h"
#include "ImGui/imgui_impl_win32.h"

const char* System::APPNAME = "Jacob's Shader Engine";

System::System()
	: mHinstance(nullptr),
	  mHwnd(nullptr),
	  mImGui(nullptr),
	  mInput(nullptr),
	  mGraphic(nullptr)
{
}

bool System::Initialize()
{
#pragma region LOGGING

	Log::Init();

#pragma endregion LOGGING

#pragma region WINDOW

	int screenWidth = 0;
	int screenHeight = 0;

	InitializeWindow(screenWidth, screenHeight);

#pragma endregion WINDOW

#pragma region GUI

	mImGui = new ImGuiManager;
	if (!mImGui)
	{
		return false;
	}
	ImGui_ImplWin32_Init(mHwnd);

#pragma endregion GUI

#pragma region INPUT

	mInput = new Input;
	if (!mInput)
	{
		return false;
	}
	mInput->Initialize();

#pragma endregion INPUT

#pragma region GRAPHIC

	mGraphic = new Graphic;
	if (!mGraphic)
	{
		return false;
	}

	if (!mGraphic->Initialize(screenWidth, screenHeight, mHwnd))
	{
		return false;
	}

	graphicCard = mGraphic->GetGraphicCardDesc();

#pragma endregion GRAPHIC

	JC_CORE_WARN("Initialization COMPLETE");

	return true;
}

void System::Exit()
{
	if (mInput)
	{
		delete mInput;
		mInput = nullptr;
	}

	if (mGraphic)
	{
		delete mGraphic;
		mGraphic = nullptr;
	}

	ImGui_ImplWin32_Shutdown();

	ExitWindow();
}

void System::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Frame();
		}
	}
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK GraphicCardINFO(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_STATIC, graphicCard);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT System::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		if (mGraphic != nullptr && wParam != SIZE_MINIMIZED)
		{
			if (mGraphic->GetD3D() == nullptr)
			{
				return 0;
			}
			mGraphic->GetD3D()->ResizeSwapChain((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
		}

		return 0;
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(mHinstance, MAKEINTRESOURCE(IDD_ABOUTBOX), mHwnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(mHwnd);
				break;
			case IDM_GRAPHIC:
				DialogBox(mHinstance, MAKEINTRESOURCE(IDD_Graphic), mHwnd, GraphicCardINFO);
				break;
			}
			return 0;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(mHwnd, &ps);
			// TODO: hdc 그리기 코드 추가
			EndPaint(mHwnd, &ps);
			return 0;
		}
	case WM_KEYDOWN:
		mInput->KeyDown(wParam);
		if (wParam == VK_ESCAPE)
			PostMessage(hWnd, WM_DESTROY, 0L, 0L);
		return 0;
	case WM_KEYUP:
		mInput->KeyUp(wParam);
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}


bool System::Frame()
{
	if (mInput->IsKeyDown(VK_ESCAPE) || mInput->IsKeyDown(VK_END))
	{
		return false;
	}

	if (!mGraphic->RenderFrame())
	{
		return false;
	}
	return true;
}

void System::InitializeWindow(int& width, int& height)
{
	gApplicationHandle = this;


#pragma region 창 클래스 등록

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW,
		MsgProc,
		0L,
		0L,
		mHinstance,
		LoadIcon(mHinstance, MAKEINTRESOURCE(IDI_SHADER)),
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		MAKEINTRESOURCE(IDC_SHADER),
		APPNAME,
		LoadIcon(mHinstance, MAKEINTRESOURCE(IDI_SMALL))
	};

	RegisterClassEx(&wc);
#pragma endregion 창 클래스 등록

	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	mHwnd = CreateWindow(
		APPNAME,
		APPNAME,
		style,
		CW_USEDEFAULT,
		0,
		width,
		height,
		GetDesktopWindow(),
		nullptr,
		mHinstance,
		nullptr);

	POINT ptDiff;
	RECT rcClient, rcWindow;
	if (FULL_SCREEN)
	{
		DEVMODE dmScreenSettings;
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(width);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(height);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		width = 1200;
		height = 800;
	}


	GetClientRect(mHwnd, &rcClient);
	GetWindowRect(mHwnd, &rcWindow);
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	MoveWindow(mHwnd, rcWindow.left, rcWindow.top, width + ptDiff.x, height + ptDiff.y, TRUE);

	ShowWindow(mHwnd, SW_SHOWDEFAULT);
	UpdateWindow(mHwnd);
}

void System::ExitWindow()
{
	DestroyWindow(mHwnd);
	mHwnd = nullptr;

	UnregisterClass(APPNAME, mHinstance);
	mHinstance = nullptr;

	gApplicationHandle = nullptr;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		return gApplicationHandle->MessageHandler(hWnd, msg, wParam, lParam);
	}
}

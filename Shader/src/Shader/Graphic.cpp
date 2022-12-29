#include "Graphic.h"
#include "Camera.h"
#include "Model.h"
#include "ColorShader.h"
#include "Log.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"


Graphic::Graphic()
	: mD3D(nullptr),
	  mCamera(nullptr),
	  mModel(nullptr),
	  mColorShader(nullptr)
{
}

bool Graphic::Initialize(float width, float height, HWND hwnd)
{
#pragma region D3D

	mD3D = new D3D(VSYNC_ENABLED);
	if (!mD3D)
	{
		return false;
	}

	if (!mD3D->Initialize(hwnd, width, height, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, "Could not Initialize Direct3D", "Error", MB_OK);
		return false;
	}

#pragma endregion D3D

#pragma region CAMERA

	mCamera = new Camera;
	if (!mCamera)
	{
		return false;
	}

	mCamera->SetPosition(0.0f, 0.0f, -10.0f);

#pragma endregion CAMERA

#pragma region MODEL

	mModel = new Model;
	if (!mModel)
	{
		return false;
	}


	if (!mModel->Initialize(mD3D->GetDevice()))
	{
		MessageBox(hwnd, LPCSTR(L"Could not initialize the model object."), LPCSTR(L"Error"), MB_OK);
		return false;
	}

#pragma endregion MODEL

#pragma region SHADER

	mColorShader = new ColorShader;
	if (!mColorShader)
	{
		return false;
	}

	if (!mColorShader->Initialize(mD3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, LPCSTR(L"Could not initialize shader object."), LPCSTR(L"Error"), MB_OK);
		return false;
	}

#pragma endregion SHADER

	ImGui_ImplDX11_Init(this->mD3D->GetDevice(), this->mD3D->GetDeviceContext());

	return true;
}

void Graphic::Clear()
{
	if (mD3D)
	{
		mD3D->Clear();
		mD3D = nullptr;
	}

	if (mColorShader)
	{
		mColorShader->Shutdown();
		delete mColorShader;
		mColorShader = nullptr;
	}

	if (mModel)
	{
		mModel->Shutdown();
		delete mModel;
		mModel = nullptr;
	}

	if (mCamera)
	{
		delete mCamera;
		mCamera = nullptr;
	}

	ImGui_ImplDX11_Shutdown();
}

bool Graphic::RenderFrame()
{
	DirectX::XMMATRIX viewMatrix, projectionMatrix, worldMatrix{};

	mD3D->BeginScene(0.401f, 0.401f, 0.401f, 1.0f);

	mCamera->Render();

	mCamera->GetViewMatrix(viewMatrix);
	mD3D->GetWorldMatrix(worldMatrix);
	mD3D->GetProjectionMatrix(projectionMatrix);

	mModel->Render(mD3D->GetDeviceContext());

	if (!mColorShader->Render(mD3D->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix, viewMatrix,
							  projectionMatrix))
	{
		return false;
	}

#pragma region IMGUI REGION

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		static float f = 0.0f;
		static int counter = 0;
		ImGui::Begin("Performance");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
					ImGui::GetIO().Framerate);
		ImGui::End();
	}

	{
		static auto cameraPos = mCamera->GetPosition();
		static auto cameraRot = mCamera->GetRotation();
		static DirectX::XMFLOAT3 posVec = {cameraPos};
		static DirectX::XMFLOAT3 rotVec = {cameraRot};

		ImGui::Begin("MainCamera");
		ImGui::Text("Position");
		ImGui::SliderFloat("x", &posVec.x, -5, 5);
		ImGui::SliderFloat("y", &posVec.y, -5, 5);
		ImGui::SliderFloat("z", &posVec.z, -15, -5);
		ImGui::Text("Rotation");
		ImGui::SliderFloat("Rotx", &rotVec.x, -180, 180);
		ImGui::SliderFloat("Roty", &rotVec.y, -180, 180);
		ImGui::SliderFloat("Rotz", &rotVec.z, -180, 180);
		mCamera->SetPosition(posVec);
		mCamera->SetRotation(rotVec);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

#pragma endregion IMGUI REGION

	mD3D->EndScene();

	return true;
}

void Graphic::RenderScene()
{
}

void Graphic::RenderInfo()
{
}

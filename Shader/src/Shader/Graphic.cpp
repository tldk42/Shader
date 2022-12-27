#include "Graphic.h"
#include "Camera.h"
#include "Model.h"
#include "ColorShader.h"
#include "Log.h"

Graphic::Graphic()
	: mD3D(nullptr),
	  mCamera(nullptr),
	  mModel(nullptr),
	  mColorShader(nullptr)
{
}

bool Graphic::Initialize(float width, float height, HWND hwnd)
{
	bool result;
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

	// Create the camera object.
	mCamera = new Camera;
	if (!mCamera)
	{
		return false;
	}

	// Set the initial position of the camera.
	mCamera->SetPosition(0.0f, 0.0f, -10.0f);

	// Create the model object.
	mModel = new Model;
	if (!mModel)
	{
		return false;
	}



	// Initialize the model object.
	result = mModel->Initialize(mD3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, LPCSTR(L"Could not initialize the model object."), LPCSTR(L"Error"), MB_OK);
		return false;
	}

	// Create the color shader object.
	mColorShader = new ColorShader;
	if (!mColorShader)
	{
		return false;
	}


	// Initialize the color shader object.
	result = mColorShader->Initialize(mD3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, LPCSTR(L"Could not initialize the color shader object."), LPCSTR(L"Error"), MB_OK);
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

	// Release the color shader object.
	if (mColorShader)
	{
		mColorShader->Shutdown();
		delete mColorShader;
		mColorShader = 0;
	}

	// Release the model object.
	if (mModel)
	{
		mModel->Shutdown();
		delete mModel;
		mModel = 0;
	}

	// Release the camera object.
	if (mCamera)
	{
		delete mCamera;
		mCamera = 0;
	}
}

bool Graphic::RenderFrame()
{
	DirectX::XMMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	mD3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	mCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	mCamera->GetViewMatrix(viewMatrix);
	mD3D->GetWorldMatrix(worldMatrix);
	mD3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mModel->Render(mD3D->GetDeviceContext());

	// Render the model using the color shader.
	result = mColorShader->Render(mD3D->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}
	mD3D->EndScene();


	// float ClearColor[4] = {0.0f, 0.125f, 0.2f, 1.0f};
	// mD3D->GetDevice()->ClearRenderTargetView(mD3D->GetRTV(), ClearColor);
	// mD3D->GetSwapChain()->Present(0, 0);
	return true;
}

void Graphic::RenderScene()
{
}

void Graphic::RenderInfo()
{
}

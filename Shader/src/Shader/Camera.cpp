#include "Camera.h"

Camera::Camera()
	: mPositionX(0),
	  mPositionY(0),
	  mPositionZ(0),
	  mRotationX(0),
	  mRotationY(0),
	  mRotationZ(0),
	  mViewMatrix()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	mPositionX = x;
	mPositionY = y;
	mPositionZ = z;
}

void Camera::SetPosition(DirectX::XMFLOAT3 vec)
{
	mPositionX = vec.x;
	mPositionY = vec.y;
	mPositionZ = vec.z;
}

void Camera::SetRotation(float x, float y, float z)
{
	mRotationX = x;
	mRotationY = y;
	mRotationZ = z;
}

void Camera::SetRotation(DirectX::XMFLOAT3 vec)
{
	mRotationX = vec.x;
	mRotationY = vec.y;
	mRotationZ = vec.z;
}

void Camera::Render()
{
	DirectX::XMFLOAT3 up{0.f, 1.f, 0.f};
	DirectX::XMFLOAT3 position{mPositionX, mPositionY, mPositionZ};
	DirectX::XMFLOAT3 lookAt{0.f, 0.f, 1.f};
	DirectX::XMMATRIX rotationMatrix{};
	float yaw;
	float pitch;
	float roll;

	pitch = mRotationX * RADIANS;
	yaw = mRotationY * RADIANS;
	roll = mRotationZ * RADIANS;

	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	DirectX::XMStoreFloat3(&lookAt, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&lookAt), rotationMatrix));
	DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&up), rotationMatrix));
	DirectX::XMStoreFloat3(
		&lookAt, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&lookAt), DirectX::XMLoadFloat3(&position)));

	mViewMatrix = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&position),
		DirectX::XMLoadFloat3(&lookAt),
		DirectX::XMLoadFloat3(&up));
}

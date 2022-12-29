#pragma once

#include <DirectXMath.h>

static constexpr float RADIANS = 0.0174532925f;

class Camera
{
public:
	Camera();
	Camera(const Camera&) = default;
	~Camera() = default;

	void SetPosition(float, float, float);
	void SetPosition(DirectX::XMFLOAT3);
	void SetRotation(float, float, float);
	void SetRotation(DirectX::XMFLOAT3);

	void Render();

	inline DirectX::XMFLOAT3 GetPosition() const
	{
		return { mPositionX, mPositionY, mPositionZ };
	}

	inline DirectX::XMFLOAT3 GetRotation() const
	{
		return { mRotationX, mRotationY, mRotationZ };
	}

	inline void GetViewMatrix(DirectX::XMMATRIX& viewMatrix) const
	{
		viewMatrix = mViewMatrix;
	};

private:
	float mPositionX;
	float mPositionY;
	float mPositionZ;
	float mRotationX;
	float mRotationY;
	float mRotationZ;
	DirectX::XMMATRIX mViewMatrix;
};

#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	Camera()noexcept;
public:
	DirectX::XMMATRIX GetViewMatrix()const noexcept;
	void SpawnControlWindow()noexcept;
	void Reset()noexcept;
	void Rotate(float dx, float dy)noexcept;
	void Orbit(float dx, float dy)noexcept;
	void Translate(DirectX::XMFLOAT3 translation)noexcept;

	DirectX::XMFLOAT3 GetPosition()const
	{
		return pos;
	}
	void SetPosition(DirectX::XMFLOAT3 v)
	{
		pos = v;
	}
	//in radians
	void SetRotation(DirectX::XMFLOAT2 rotation)
	{
		rot = rotation;
	}
	DirectX::XMFLOAT3 TransformToView(DirectX::XMFLOAT3 pos)const
	{
		namespace dx = DirectX;
		dx::XMStoreFloat3(&pos, dx::XMVector3Transform(
			dx::XMLoadFloat3(&pos),
			dx::XMMatrixRotationRollPitchYaw(0.0f, rot.y, 0.0f) *
			dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
		));
		return pos;
	}
private:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 rot;
	DirectX::XMFLOAT2 orbit;
	static constexpr const float travelSpeed = 12.0f;
	static constexpr const float rotationSpeed = 0.004f;
};

class StereoCamera
{
public:
	StereoCamera()noexcept;
public:
	DirectX::XMMATRIX GetLeftViewMatrix()const noexcept;
	DirectX::XMMATRIX GetRightViewMatrix()const noexcept;
	DirectX::XMMATRIX GetCentralViewMatrix()const noexcept;

	void SpawnControlWindow()noexcept;
	void Reset()noexcept;
	void Rotate(float dx, float dy)noexcept;
	void Orbit(float dx, float dy)noexcept;
	void Translate(DirectX::XMFLOAT3 translation)noexcept;

	DirectX::XMFLOAT3 GetPosition()const
	{
		return pos;
	}
	void SetPosition(DirectX::XMFLOAT3 v)
	{
		pos = v;
	}
	//in radians
	void SetRotation(DirectX::XMFLOAT2 rotation)
	{
		rot = rotation;
	}
	DirectX::XMFLOAT3 TransformToView(DirectX::XMFLOAT3 pos)const
	{
		namespace dx = DirectX;
		dx::XMStoreFloat3(&pos, dx::XMVector3Transform(
			dx::XMLoadFloat3(&pos),
			dx::XMMatrixRotationRollPitchYaw(0.0f, rot.y, 0.0f) *
			dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
		));
		return pos;
	}
private:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 rot;

	float eye_dist = 0.65f;
	float focus_distance = 100.0f;
	int conv = 0;

	static constexpr const float travelSpeed = 12.0f;
	static constexpr const float rotationSpeed = 0.004f;
};
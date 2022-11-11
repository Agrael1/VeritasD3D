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

	DirectX::XMFLOAT3 position()
	{
		return pos;
	}
	void set_position(DirectX::XMFLOAT3 v)
	{
		pos = v;
	}
	DirectX::XMFLOAT3 TransformToView(DirectX::XMFLOAT3 pos)const
	{
		pos.y = 0.0f;
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